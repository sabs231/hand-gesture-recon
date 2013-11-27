#include <iostream>
#include <fstream>
#include <sstream>
#include "opencv/cv.h"
#include <opencv2/opencv.hpp>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <map>
#define DEBUG 0
const short COMPUTE_FRAME_THRESHOLD = 0; // How many frames should pass to compute a vector
const double MOTION_HISTORY_SENSITIVITY = 50; // ORIG 45
const double MHI_DURATION = 1;
const double MGO_DURATION = 1;

void fillVideoMap(std::multimap<char,std::string> *videoFileClass){
	std::ostringstream fileNameStream;
	// Hola
	for(int i = 0; i < 31; i++){
		if(i < 10){
			fileNameStream << "hola0" << i;
		}else{
			fileNameStream << "hola" << i;
		}
		videoFileClass->insert (std::pair<char,std::string>('H',fileNameStream.str()));
		fileNameStream.str("");
		fileNameStream.clear();
	}
	// Matrimonio
	for(int i = 1; i < 41; i++){
		if(i < 10){
			fileNameStream << "matrimonio0" << i;
		}else{
			fileNameStream << "matrimonio" << i;
		}
		videoFileClass->insert (std::pair<char,std::string>('M',fileNameStream.str()));
		fileNameStream.str("");
		fileNameStream.clear();
	}
	// Querer
	for(int i = 1; i < 41; i++){
		if(i < 10){
			fileNameStream << "querer0" << i;
		}else{
			fileNameStream << "querer" << i;
		}
		videoFileClass->insert (std::pair<char,std::string>('Q',fileNameStream.str()));
		fileNameStream.str("");
		fileNameStream.clear();
	}
}

// parameters:
//  img - Input Original Image
//  dst - Resultant Motion Picture
//  mhi - Motion History Image
//  buf - MHI ring image buffer
//  silh- MHI Silhouete
//  mask- MHI Mask
//  diff_threshold - optional parameters
static void  update_mhi( IplImage* img, IplImage* dst, IplImage** mhi, IplImage*** buf, IplImage **silh, IplImage **mask, int diff_threshold, int *last){
	const int N 		= 4; // Number of cyclic frame buffer used for motion detection (should, probably, depend on FPS)
	double timestamp	= (double)clock()/CLOCKS_PER_SEC; // get current time in seconds
	CvSize size 		= cvSize(img->width,img->height); // get current frame size
	int i, idx1 		= (*last), idx2;

	// Allocate images at the beginning or reallocate them if the frame size is changed
	if( !(*mhi) || (*mhi)->width != size.width || (*mhi)->height != size.height ) {
		if( (*buf) == 0 ) {
			(*buf) = (IplImage**)malloc(N*sizeof((*buf[0])));
			memset( (*buf), 0, N*sizeof((*buf)[0]));
		}
		for( i = 0; i < N; i++ ) {
			cvReleaseImage( &(*buf)[i] );
			(*buf)[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
			cvZero( (*buf)[i] );
		}
		cvReleaseImage( mhi );
		cvReleaseImage( mask );
		(*mhi) = cvCreateImage( size, IPL_DEPTH_32F, 1 );
		cvZero( (*mhi) ); // clear MHI at the beginning
		(*mask) = cvCreateImage( size, IPL_DEPTH_8U, 1 );
	}
	if(!(*mask)){
		(*mask) = cvCreateImage( size, IPL_DEPTH_8U, 1 );
	}
	cvCvtColor( img, (*buf)[(*last)], CV_BGR2GRAY ); // convert frame to grayscale
	idx2 = ((*last) + 1) % N; // index of (last - (N-1))th frame
	(*last) = idx2;
	(*silh) = (*buf)[idx2];
	cvAbsDiff( (*buf)[idx1], (*buf)[idx2], (*silh) ); // get difference between frames
	cvThreshold( (*silh), (*silh), diff_threshold, 255, CV_THRESH_BINARY); // and threshold it
	cvUpdateMotionHistory( (*silh), (*mhi), timestamp, MHI_DURATION ); // update MHI
	
	// convert MHI to blue 8u image
	cvCvtScale( (*mhi), (*mask), 255./MHI_DURATION, (MHI_DURATION - timestamp)*255./MHI_DURATION );
	cvZero( dst );
	cvMerge( (*mask), 0, 0, 0, dst );
}

// parameters:
std::vector<double> computeVectors(IplImage** mhi, IplImage* dst, short wROI, short hROI){
	double timestamp = (double)clock()/CLOCKS_PER_SEC; // Get current time in seconds
	CvSize size = cvSize(dst->width, dst->height); // Get current frame size; 640x480
	std::vector<double> relevanceVector;
	CvRect roi;
	double count;
	double angle;
	CvPoint center;
	double magnitude;
	CvScalar color;
	CvRect comp_rect;
	IplImage *orient = 0; // MGO Orientation
	IplImage *mask = 0;   // MGO valid orientation mask
	const double MAX_TIME_DELTA = 0.5;
	const double MIN_TIME_DELTA = 0.05;

	// Calculate Motion Gradient Orientation and valid orientation mask
	orient	= cvCreateImage( size, IPL_DEPTH_32F, 1 );
	mask	= cvCreateImage( size, IPL_DEPTH_8U, 1 );
	cvCalcMotionGradient( (*mhi), mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3 );
	
	// Compute Global Motion
	comp_rect 	= cvRect( 0, 0, size.width, size.height );
	color 		= CV_RGB(255,255,255);
	magnitude 	= 100;
	angle 		= cvCalcGlobalOrientation( orient, mask, (*mhi), timestamp, MGO_DURATION);
	relevanceVector.push_back(angle);
	angle 		= 360.0 - angle;  // adjust for images with top-left origin
	roi 		= cvGetImageROI((*mhi));
	center 		= cvPoint( (comp_rect.x + comp_rect.width/2), (comp_rect.y + comp_rect.height/2) );
	cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
	cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
	cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );

	// Compute Motion on perimetral cuadrants
	int iwidth = (dst->width / wROI);
	int iheight = (dst->height / hROI);
	int w = 0;
	color = CV_RGB(255,0,0);
	magnitude = 30;
	while(w < iwidth){
		// Top
			comp_rect.x = w*wROI;
			comp_rect.y = 0;
			comp_rect.width = wROI;
			comp_rect.height = hROI;
			cvSetImageROI( (*mhi), comp_rect );
			cvSetImageROI( orient, comp_rect );
			cvSetImageROI( mask, comp_rect );
			// Process Motion
			angle = cvCalcGlobalOrientation( orient, mask, (*mhi), timestamp, MHI_DURATION);
			relevanceVector.push_back(angle);
			angle = 360.0 - angle;  // adjust for images with top-left origin
			roi = cvGetImageROI((*mhi));
			center = cvPoint( (comp_rect.x + comp_rect.width/2),
					  (comp_rect.y + comp_rect.height/2) );
			cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
			cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
			cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );
			cvResetImageROI( (*mhi) );
			cvResetImageROI( orient );
			cvResetImageROI( mask );
		// Bottom
			comp_rect.x = w*wROI;
			comp_rect.y = dst->height - hROI;
			cvSetImageROI( (*mhi), comp_rect );
			cvSetImageROI( orient, comp_rect );
			cvSetImageROI( mask, comp_rect );
			// COMPUTE
			angle = cvCalcGlobalOrientation( orient, mask, (*mhi), timestamp, MHI_DURATION);
			relevanceVector.push_back(angle);
			angle = 360.0 - angle;  // adjust for images with top-left origin
			roi = cvGetImageROI((*mhi));
			center = cvPoint( (comp_rect.x + comp_rect.width/2),
					  (comp_rect.y + comp_rect.height/2) );
			cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
			cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
			cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );
			cvResetImageROI( (*mhi) );
			cvResetImageROI( orient );
			cvResetImageROI( mask );
		w++;
	}
	int h = 1;
	while(h < (iheight-1)){
		// Left
			comp_rect.x = 0;
			comp_rect.y = h*hROI;
			comp_rect.width = wROI;
			comp_rect.height = hROI;
			cvSetImageROI( (*mhi), comp_rect );
			cvSetImageROI( orient, comp_rect );
			cvSetImageROI( mask, comp_rect );
			// Process Motion
			angle = cvCalcGlobalOrientation( orient, mask, (*mhi), timestamp, MHI_DURATION);
			relevanceVector.push_back(angle);
			angle = 360.0 - angle;  // adjust for images with top-left origin
			roi = cvGetImageROI((*mhi));
			center = cvPoint( (comp_rect.x + comp_rect.width/2),
					  (comp_rect.y + comp_rect.height/2) );
			cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
			cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
			cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );
			cvResetImageROI( (*mhi) );
			cvResetImageROI( orient );
			cvResetImageROI( mask );
		// Right
			comp_rect.x = dst->width - wROI;
			comp_rect.y = h*hROI;
			cvSetImageROI( (*mhi), comp_rect );
			cvSetImageROI( orient, comp_rect );
			cvSetImageROI( mask, comp_rect );
			// Process Motion
			angle = cvCalcGlobalOrientation( orient, mask, (*mhi), timestamp, MHI_DURATION);
			relevanceVector.push_back(angle);
			angle = 360.0 - angle;  // adjust for images with top-left origin
			roi = cvGetImageROI((*mhi));
			center = cvPoint( (comp_rect.x + comp_rect.width/2),
					  (comp_rect.y + comp_rect.height/2) );
			cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
			cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
			cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );
			cvResetImageROI( (*mhi) );
			cvResetImageROI( orient );
			cvResetImageROI( mask );
		h++;
	}
	
	// Compute Motion on 4x4 Cuadrants
	// Sorry for the HardCoding, lack of time
	
	color = CV_RGB(255,0,0);
	magnitude = 20;
	for (int r = hROI; r < size.height - hROI; r += 72){
		for (int c = wROI; c < size.width - wROI; c += 96){
			comp_rect.x = c;
			comp_rect.y = r;
			comp_rect.width = 96;
			comp_rect.height = 72;

			cvSetImageROI( (*mhi), comp_rect );
			cvSetImageROI( orient, comp_rect );
			cvSetImageROI( mask, comp_rect );

			// Process Motion
			angle = cvCalcGlobalOrientation( orient, mask, (*mhi), timestamp, MHI_DURATION);
			relevanceVector.push_back(angle);
			
			angle = 360.0 - angle;  // adjust for images with top-left origin
			roi = cvGetImageROI((*mhi));
			center = cvPoint( (comp_rect.x + comp_rect.width/2),
					  (comp_rect.y + comp_rect.height/2) );
			cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
			cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
			cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );	
			
			cvResetImageROI( (*mhi) );
			cvResetImageROI( orient );
			cvResetImageROI( mask );
		}
	}

	if(DEBUG){
		for (std::vector<double>::iterator it = relevanceVector.begin() ; it != relevanceVector.end(); ++it)
			std::cout << (*it) << ", ";
		std::cout << std::endl;
	}
	
	return relevanceVector;
}

int main(int argc, char** argv){
	/* Variables Globales */
	IplImage*	motion = 0;
	IplImage*	captionOriginalImage = 0;
	IplImage*	mhi = 0; // MHI
	IplImage*	silh = 0; // MHI Silhuete
	IplImage**	buf = 0; // MHI ring image buffer
	IplImage*	mask = 0; // MHI valid orientation mask
	CvCapture*	capture = 0;
	short 		frameCount = 0;
	int 		lastHistoryFrame = 0;
	double 		fps	= 0.0;
	std::ofstream 	relevanceVectorFile;

	// What to DO
	short train = 0;
	if( argc == 2 && strlen(argv[1]) == 5){
		train = 1;
	}
	
	if(train){
		std::vector< std::vector<double> > relevanceVectors;
		std::vector<double> relevanceTmp;
		std::ostringstream fileNameStream;
		// Mapa de Videos
		std::multimap<char,std::string> videoFileClass;
		fillVideoMap(&videoFileClass);
		//videoFileClass.insert(std::pair<char,std::string>('H',"hola00"));
		//videoFileClass.insert(std::pair<char,std::string>('H',"hola01"));
		// Iterar por cada uno de los videos y obtener su vector de Relevancia
		cvNamedWindow( "Motion", 1 );
		relevanceVectorFile.open ("relevanceVectors.txt");
		for (std::multimap<char,std::string>::iterator it=videoFileClass.begin(); it!=videoFileClass.end(); ++it){
			std::cout << "-- INICIANDO ANALISIS DE " << (*it).first << " - " << (*it).second << std::endl;
			fileNameStream << "../_videos/" << (*it).second << ".avi";
			capture = cvCaptureFromFile( fileNameStream.str().c_str() );
			fileNameStream.str("");
			fileNameStream.clear();
			if(capture){
				fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
				std::cout << "FPS: " <<  fps << std::endl;
				while(true){
					captionOriginalImage = cvQueryFrame( capture );
					if(!captionOriginalImage) break;
					if( !motion ){
						motion = cvCreateImage( cvSize(captionOriginalImage->width,captionOriginalImage->height), 8, 3);
						cvZero( motion );
						motion->origin = captionOriginalImage->origin;
					}
					++frameCount;
					if(frameCount > COMPUTE_FRAME_THRESHOLD){
						frameCount = 0;
						update_mhi( captionOriginalImage, motion, &mhi, &buf, &silh, &mask, MOTION_HISTORY_SENSITIVITY, &lastHistoryFrame);
		    			relevanceVectors.push_back(computeVectors(&mhi, motion, 128, 96));
					}
					cvShowImage( "Motion", motion );
					if(cvWaitKey(10) >= 0) break;
				}
				fileNameStream << "../_generatedImages/" << (*it).second  << ".jpg"; // write to string stream
				std::string file_name = fileNameStream.str(); // get string out of stream
				fileNameStream.str("");
				fileNameStream.clear();
				cvSaveImage(file_name.c_str(), motion);
				cvReleaseCapture( &capture );
				// Save the Relevance Vectors
				int sizeVector = relevanceVectors.size();
				int sizeMiddle = sizeVector/2;
				// Middle
				relevanceTmp = relevanceVectors[sizeMiddle];
				relevanceVectorFile << (*it).first;
				for (std::vector<double>::iterator it = relevanceTmp.begin() ; it != relevanceTmp.end(); ++it){
						relevanceVectorFile << ", " << (*it);
				}
				relevanceVectorFile << std::endl;
				// Last
				relevanceTmp = relevanceVectors[sizeVector-1];
				relevanceVectorFile << (*it).first;
				for (std::vector<double>::iterator it = relevanceTmp.begin() ; it != relevanceTmp.end(); ++it){
						relevanceVectorFile << ", " << (*it);
				}
				relevanceVectorFile << std::endl;
				// M-Last
				relevanceTmp = relevanceVectors[((sizeVector-sizeMiddle)/2)+sizeMiddle];
				relevanceVectorFile << (*it).first;
				for (std::vector<double>::iterator it = relevanceTmp.begin() ; it != relevanceTmp.end(); ++it){
						relevanceVectorFile << ", " << (*it);
				}
				relevanceVectorFile << std::endl;
				relevanceVectors.clear();
				
			}
		}
		cvDestroyWindow( "Motion" );
		relevanceVectorFile.close();
	}else{
		cvNamedWindow( "Motion", 1 );	
			capture = cvCaptureFromCAM(0);
			if(capture){
				fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
				std::cout << "FPS: " <<  fps << std::endl;
				while(true){
					captionOriginalImage = cvQueryFrame( capture );
					if(!captionOriginalImage) break;
					if( !motion ){
						motion = cvCreateImage( cvSize(captionOriginalImage->width,captionOriginalImage->height), 8, 3);
						cvZero( motion );
						motion->origin = captionOriginalImage->origin;
					}
					++frameCount;
					if(frameCount > COMPUTE_FRAME_THRESHOLD){
						frameCount = 0;
						update_mhi( captionOriginalImage, motion, &mhi, &buf, &silh, &mask, MOTION_HISTORY_SENSITIVITY, &lastHistoryFrame);
						computeVectors(&mhi, motion, 128, 96);
					}
					cvShowImage( "Motion", motion );
					if(cvWaitKey(10) >= 0) break;
				}
				cvReleaseCapture( &capture );
			}
		cvDestroyWindow( "Motion" );
	}

	return 0;
}