import getopt, sys
import numpy as np
import cv2
import main






#def testimg(sysargv):

for tc in range(0,100):
#for tc in tess:
#for tc in prob:
	#kk=str(tc)+"_Junk.jpg"
	kk=str(tc)+"_0__crop.jpg"
	#kk=str(tc)+"_0__crop.png"
	print "\n"+kk

	
	loaded_image_mat = cv2.imread(kk)

	
	#====================================================================
	#	segmentation
	(shapeSeg, shapeColor, charSeg, charColor) = main.doSegmentation(loaded_image_mat, 0,tc)
	
	#====================================================================
	#	display image and results
	
#"""
	cv2.imshow(kk, loaded_image_mat)
	cv2.imshow("shape seg", shapeSeg*255) #images are floating point from 0.0 to 255.0, convert from 0.0 to 1.0 for imshow
	cv2.imshow("char seg", charSeg*255)
	cv2.waitKey(0) #wait for keypress

#"""
"""



def testimg(sysargv):
	#====================================================================
	#	filename of image to load is a command line argument
	
	if len(sysargv) <= 1:
		print("usage:  {image-file}  {optional:show-images?}")
		quit()
	
	filename_of_image = str(sysargv[1])
	showImages = True
	if len(sysargv) > 2:
		if int(sysargv[2]) == 0:
			showImages = False
	
	#====================================================================
	#	load the image
	
	loaded_image_mat = cv2.imread(filename_of_image)
	
	#check if it failed to load
	if np.size(loaded_image_mat) <= 1:
		print "ERROR: COULD NOT OPEN IMAGE FILE: " + filename_of_image
		return
	
	#====================================================================
	#	segmentation
	
	(shapeSeg, shapeColor, charSeg, charColor) = main.doSegmentation(loaded_image_mat, 0)
	
	#====================================================================
	#	display image and results
	
	if showImages:
		cv2.imshow("original image", loaded_image_mat)
		cv2.imshow("shape seg", shapeSeg*255.0) #images are floating point from 0.0 to 255.0, convert from 0.0 to 1.0 for imshow
		cv2.imshow("char seg", charSeg*255.0)
		cv2.waitKey(0) #wait for keypress
	


# execute main()... this needs to be at the end
if __name__ == "__main__":
	testimg(sys.argv)
#"""
