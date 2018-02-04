#include <cstdio>
#include "bitmap_image.hpp"
#include <vector>
#include <cmath>

using namespace std;

bitmap_image noir_ou_blanc(bitmap_image image);
bitmap_image soustraire_images(bitmap_image image1, bitmap_image image2);
bitmap_image convolution(bitmap_image image1, vector<vector<int> > matrice);
void recherche_hauteur (bitmap_image image, int y, int x1, int x2, int* coordonees);
void rectangles_blancs(bitmap_image image);

void verif_rectangle(bitmap_image image, int x1, int x2, int y1, int y2);
int moyenne_bord_x(bitmap_image image, int tolerance, int coord);

int moyenne_bord_y(bitmap_image image, int tolerance, int coord);
int recherche_blobs(bitmap_image image, int* tableau_blob_x, int* tableau_blob_y);
bool est_un_rectangle (int largeur, int hauteur, int tolerance, int plusPetiteDiagonale, int plusGrandeDiagonale);


int main()
{

   //int tab_erosion[3][3] = {{0, 1, 0},{1, 1, 1},{0, 1, 0}};
   //int (*erosion)[3] = tab_erosion;

   vector<vector<int> > erosion(3, vector<int>(3,0));
   erosion = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};

	//vector<vector<int> > erosion {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};

   bitmap_image image1("avecflash24t.bmp");
   bitmap_image image2("ssflash24t.bmp");
   if (!image1)
   {
      printf("Error - Failed to open: input1.bmp\n");
      return 1;
   }

   if (!image2)
   {
      printf("Error - Failed to open: input2.bmp\n");
      return 1;
   }

   image1 = noir_ou_blanc(image1);
   image1.save_image("1.bmp");
   image2 = noir_ou_blanc(image2);
   image2.save_image("2.bmp");
   image1 = soustraire_images(image1, image2);
   image1.save_image("3.bmp");
   image1 = convolution(image1, erosion);
   image1 = noir_ou_blanc(image1);
   image1 = convolution(image1, erosion);
   image1 = noir_ou_blanc(image1);
   image1 = convolution(image1, erosion);   
   image1 = noir_ou_blanc(image1);
   image1.save_image("resultat_glorieux.bmp");
   cout<<"Resultat Glorieux !"<<endl;
   rectangles_blancs(image1);
   return 0;

}



//deux images en noir ou blanc
//image1 = image1(led) - image2(ss led)
//image2 doit etre de meme taille ou plus grande
bitmap_image soustraire_images(bitmap_image image1, bitmap_image image2)
{
    const unsigned int height = image1.height();
    const unsigned int width  = image1.width();

    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {
            rgb_t colour1;
            rgb_t colour2;

            image1.get_pixel(x, y, colour1);
            image2.get_pixel(x, y, colour2);
            if (colour2 == colour1)
                image1.set_pixel(x, y, 0, 0, 0);
        }
    }
    return image1;
}



//Passe l'image en noir ou blanc
bitmap_image noir_ou_blanc(bitmap_image image)
{
    const unsigned int height = image.height();
    const unsigned int width  = image.width();

    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {
            rgb_t colour;

            image.get_pixel(x, y, colour);
            if ( colour.red * 299/1000 + colour.green * 587/1000 + colour.blue * 114/1000 > 200)// ITU-R 601-2 et seuil a 200
            {
                image.set_pixel(x, y, 255, 255, 255);
            }
            else
            {
                image.set_pixel(x, y, 0, 0, 0);
            }
        }
    }
    return image;
}




//Convolution par une matrice 3:3
bitmap_image convolution(bitmap_image image, vector<vector<int> > matrice)
{
    const unsigned int height = image.height();
    const unsigned int width  = image.width();

    for (std::size_t y = 1; y < height-1; ++y)
    {
        for (std::size_t x = 1; x < width-1; ++x)
        {
            rgb_t colour1;
            rgb_t colour2;
            rgb_t colour3;
            rgb_t colour4;
            rgb_t colour5;
            rgb_t colour6;
            rgb_t colour7;
            rgb_t colour8;
            rgb_t colour9;

            rgb_t colour;

            image.get_pixel(x-1, y-1, colour1);
            image.get_pixel(x-1, y, colour2);
            image.get_pixel(x-1, y+1, colour3);
            image.get_pixel(x, y-1, colour4);
            image.get_pixel(x, y, colour5);
            image.get_pixel(x, y+1, colour6);
            image.get_pixel(x+1, y-1, colour7);
            image.get_pixel(x+1, y, colour8);
            image.get_pixel(x+1, y+1, colour9);

            colour.red = (matrice[0][0]*colour1.red + matrice[0][1]*colour2.red + matrice[0][2]*colour3.red + matrice[1][0]*colour4.red + matrice[1][1]*colour5.red + matrice[1][2]*colour6.red + matrice[2][0]*colour7.red + matrice[2][1]*colour8.red + matrice[2][2]*colour9.red)/9;

            colour.green = colour.red;
            colour.blue = colour.red;

            image.set_pixel(x, y, colour);
        }
    }
    return image;
}


int recherche_blobs(bitmap_image image, int* tableau_blob_x, int* tableau_blob_y)
{
	int nombre_blob = 0;
	int coordonees[2];
	//On boucle dans l'image à une hauteur intermediaire pour chercher les pixels blancs
	const unsigned int height = image.height();
    const unsigned int width  = image.width();
    rgb_t couleur;
    int y = image.height()/2;
    y = 400;	//############################################################################ A CORRIGER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    int debut_blob = -10;
    int fin_blob = -10;
    bool blob = false;
    cout<<"recherche de rectangles"<<endl;
    for (std::size_t x = 0; x < width; ++x)
    {
        image.get_pixel(x, y, couleur);
        //cout<<(int)couleur.red<<endl;
        if (couleur.red == 255 && blob == false)
        {
            blob = true;
            debut_blob = x;
            cout<<"debut blob"<<endl;
        }
        if (couleur.red == 0 && blob == true)
        {
            blob = false;
            fin_blob = x - 1;
            cout<<"fin blob"<<endl;
        }
        if (debut_blob > 0 && fin_blob > 0)
        {
            //coordonees = recherche_hauteur(image, y, debut_blob, fin_blob);
	    //moyenne_bord_y();
	    	int prem = nombre_blob*2;
		int deux = nombre_blob*2+1;
	    	tableau_blob_x[prem] = debut_blob;
		tableau_blob_x[deux] = fin_blob;
		nombre_blob +=1;
           	blob = false;
            	debut_blob = -10;
            	fin_blob = -10;
        	}
	}
    	return nombre_blob;
}


//TODO
//fonction moyenne x et y
//determinatio y
//audit est_un_rectangle
//
//
//
//





void rectangles_blancs(bitmap_image image)
{
	cout<<"########################################################### Debut Reco Visu"<<endl;
	int tableau_blob_x[100];
	int tableau_blob_y[100];
	cout<<"recherche de blobs"<<endl;
	int nombre_blob = recherche_blobs(image, tableau_blob_x, tableau_blob_y);
	cout<<nombre_blob<<" blobs trouve"<<endl;
	int i = 0;
	int y = 400;										//CONSTANTE MAGIQUE A DETERMINEER QCIENTIFIQUEMENT
	int coordonees[2];
	int tableau_bandes[100];
	int nbrBandes= 0;
	for (i = 0; i <nombre_blob; i++)
	{
		cout<<"######## Blob n"<<i<<endl;
		cout<<"largeur blob "<<tableau_blob_x[i*2]<<" ; "<< tableau_blob_x[i*2+1]<<endl;
		recherche_hauteur(image,y,  tableau_blob_x[i*2], tableau_blob_x[i*2+1], coordonees);
		cout<<"hauteur blob n "<<i<<" : "<<coordonees[0]<<"  "<<coordonees[1]<<endl;
		tableau_blob_x[i*2] = moyenne_bord_x(image, 5, tableau_blob_x[i*2]);
		tableau_blob_x[i*2+1] = moyenne_bord_x(image, 5, tableau_blob_x[i*2+1]);
		tableau_blob_y[i*2] = moyenne_bord_y(image, 5, coordonees[0]);
		tableau_blob_y[i*2+1] = moyenne_bord_y(image, 5, coordonees[1]);
	}
	//on a un beau tableau de blobs propres
	for (i = 0; i < nombre_blob; i++)//on s'assure que nos blobs sont de beaux rectangles avec de bonnes proportions et compris entre deux bornes
	{
		int largeur = tableau_blob_x[i*2+1]-tableau_blob_x[i*2];
		int hauteur = tableau_blob_y[i*2+1]-tableau_blob_y[i*2];
		if (est_un_rectangle(largeur, hauteur, 5, 10, 400))// tolerance et bornes min et max //A VERIFIER VRAI%MENT #################################################""
		{
			nbrBandes +=1;
			tableau_bandes[] = tableau_blob_x[i*4], 
				tableau_bandes[] = tableau_blob_x[], 
				tableau_blob_y[], 
				tableau_blob_y[]);
		}
	}	
	if (nbrBandes != 2)
	{
		cout<<"ALEEEEERRRTE !!!!!!!!!!!!!!!!!!! NOUS AVONS UN CODE ROUGE !!!!!!"<<endl;
	}
	else
	{
		cout<<"tous vas bien, on a ddeux bandes"<<endl;
	}
}	


void recherche_hauteur (bitmap_image image, int y, int x1, int x2, int* coordonees)//image, hauteur arbitraire y du rectangle, xdebut rectangle, x fin rectangle
{
    const unsigned int height = image.height();
    rgb_t colour;
    //int coordonees[2];
    int x = (x1 + x2)/2;
    bool prems = true;
    
    for (std::size_t y = 0; y < height; ++y)
	{
		image.get_pixel(x, y, colour);
		if (colour.red == 255)
		{
			coordonees[1] = y;
			if(prems == true){coordonees[0] = y;prems = false;}
		}
	}  
    
    /*
    image.get_pixel(x, y, colour);

    int y1 = y;
    while (colour.red == 255)
    {
        image.get_pixel(x, y1, colour);
        y1--;
    }
    coordonees[0] = y1-1;

    image.get_pixel(x, y, colour);

    int y2 = y;
    while (colour.red == 255)
    {
        image.get_pixel(x, y2, colour);
        y2++;
    }

    coordonees[1] = y2+1;
		
    cout<<"coordonnes du rectangle : "<<x1<<","<<x2<<","<<coordonees[0]<<","<<coordonees[1]<<endl;
*/
   // verif_rectangle(image, x1, x2, coordonees[0], coordonees[1]);
    
}

void verif_rectangle(bitmap_image image, int x1, int x2, int y1, int y2)
{
	int coordonees[2];
	x1 = moyenne_bord_x(image, 5, x1);
	x2 = moyenne_bord_x(image, 5, x2);
	//y1 = moyenne_bord_y(image, 5, y1);
	//y2 = moyenne_bord_y(image, 5, y2);
	
	coordonees[1] = y2;

    cout<<"coordonnes du rectangle : "<<x1<<","<<x2<<","<<coordonees[0]<<","<<coordonees[1]<<endl;

    int largeur = x2 - x1;
    int hauteur = y2 - y1;
    cout<<"Est un rectangle : "<<est_un_rectangle(largeur, hauteur, 50, 0, 1000) << endl; //Tolerance et Tailles de diagonales à determiner

}




int moyenne_bord_y(bitmap_image image, int tolerance, int coord)
{
	return coord;
}

int moyenne_bord_x(bitmap_image image, int tolerance, int coord)
{

	const unsigned int height = image.height();
	const unsigned int width  = image.width();
	
	int tab_densite[15];
	int densitee = 0;
	rgb_t colour;
	
	for (std::size_t x = coord - tolerance; x < coord + tolerance; ++x)
   	{
		for (std::size_t y = 0; y < height; ++y)
		{
			image.get_pixel(x, y, colour);
			if (colour.red == 255)
			{
				densitee++;
			}
		}
		tab_densite[x - (coord - tolerance)] = densitee;
		cout<<densitee<<",";
		densitee = 0;
	}
	cout<<endl;
	//for (int i = 0; i < tolerance; ++i) ATTENTION TABLEAU CONTIENT PAS BONNES VALEURS, BIZARRE
	//{
		//cout<<tab_densite[i]<<",";
	//}
	//cout<<endl;
	return coord;


}

//Si la hauteur est environ 9X plus grande (normalement c'est 8X mais avec l'erosion la largeur diminue) que la largeur et que la diagonale est comprise entre 2 bornes -> true
bool est_un_rectangle (int largeur, int hauteur, int tolerance, int plusPetiteDiagonale, int plusGrandeDiagonale)
{
    double diagonale = sqrt( pow(largeur,2) + pow(hauteur,2) );
    cout<<"Diagonale : "<< diagonale<<endl;
    return (9*largeur < hauteur+tolerance && 9*largeur > hauteur-tolerance) && (diagonale < plusGrandeDiagonale && diagonale > plusPetiteDiagonale);

}
