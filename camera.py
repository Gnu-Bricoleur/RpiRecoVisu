import cv2
import os
import time
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)
GPIO.setup(4, GPIO.OUT)
cam = cv2.VideoCapture()
cam.open(-1)
i = 0

for j in range(10):
	img=cam.read()
	cv2.imwrite("Go.bmp",img[1])

while i<10:
	#prendre photo
	img=cam.read()
	cv2.imwrite("ssflash24tg.bmp",img[1])
	#os.system("convert imgssflsh.jpg -type truecolor -depth 8 BMP3:ssflash24tg.bmp")
	#allumer lumiere
	GPIO.output(4, 1)
	#prendre photo
	img=cam.read()
	cv2.imwrite("avecflash24tg.bmp",img[1])
	#os.system("convert imgacflsh.jpg -type truecolor -depth 8 BMP3:avecflash24tg.bmp")
	#eteindre lumiere
	GPIO.output(4, 0)
	#traiter image
	os.system("./a.out")
	#communiquer roborio
	print "##################################################################################################################################################yololo"
	print "##################################################################################################################################################yololo"
	print "##################################################################################################################################################yololo"
	print "##################################################################################################################################################yololo"
	print "##################################################################################################################################################yololo"
	i=i+1
	#time.sleep(20)
	#print "Go"
	#time.sleep(5)
