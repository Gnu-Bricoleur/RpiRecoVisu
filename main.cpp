#include <cstdio>
#include "bitmap_image.hpp"
#include <vector>

using namespace std;

bitmap_image noir_ou_blanc(bitmap_image image);
bitmap_image soustraire_images(bitmap_image image1, bitmap_image image2);
bitmap_image convolution(bitmap_image image1, vector<vector<int> > matrice);

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
   image1.save_image("resultat_glorieux.bmp");
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

void rectangles_blancs(bitmap_image image)
{
	//On boucle dans l'image à une hauteur intermediaire pour chercher les pixels blancs
	const unsigned int height = image.height();
	const unsigned int width  = image.width();
	rgb_t couleur;
	int y = image.height()/2;
	int debut_blob = -10;
	int fin_blob = -10;
	bool blob = false;
	
	for (std::size_t x = 0; x < width; ++x)
	{
		image.get_pixel(x, y, couleur);
		if (couleur.red == 255 && blob == false)
		{
			blob = true;
			debut_blob = x;
		}
		if (couleur.red == 0 && blob == true)
		{
			blob = false;
			fin_blob = x - 1;
		}
		if (debut_blob > 0 && fin_blob > 0)
		{
			recherche_hauteur(image, y, debut_blob, fin_blob);
			blob = false;
			debut_blob = -10;
			fin_blob = -10;
		}
		
	}
	
	
}

void moyenne_bord(bitmap_image image, int tolerance, int coord)
{
	
}
