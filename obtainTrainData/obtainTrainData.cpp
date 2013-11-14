#include <iostream>
#include "opencv/cv.h"
#include <opencv2/opencv.hpp>
#include <time.h>
#include <stdio.h>
#include <ctype.h>

#define DEBUG 0

// various tracking parameters (in seconds)
const double MHI_DURATION = 14;
const double MAX_TIME_DELTA = 0.5;
const double MIN_TIME_DELTA = 0.05;
const double MOTION_HISTORY_SENSITIVITY = 45;//35
const double MOTION_VECTOR_SENSITIVITY = 150;//100
const double SURF_THRESHOLD = 100;//500
const short COMPUTE_FRAME_THRESHOLD = 5;// How many frames should pass to compute a vector
const int N = 4; // 4 number of cyclic frame buffer used for motion detection (should, probably, depend on FPS)

// ring image buffer
IplImage **buf = 0;
int last = 0;

// temporary images
IplImage *silh = 0;
IplImage *mhi = 0; // MHI
IplImage *orient = 0; // orientation
IplImage *mask = 0; // valid orientation mask
IplImage *segmask = 0; // motion segmentation map
CvMemStorage* storage = 0; // temporary storage

// Relevance Vectors
/*
	25 Relevancia
	25 Direccion
	1 Direccion
	0 top
	1 bottom
	2 right
	3 left
	4 top-right
	5 top-left
	6 bottom-right
	7 bottom-left
	
*/
short relevanceVector[51];

void initMatrix(short relevanceVector[51], short size){
	for(int i=0; i<size; i++)
		relevanceVector[i]=0;
}

void relevancePointToVector(int x, int y, short wROI, short hROI, short vectorSize){
	int pow = 0;
	int poh = 0;
	for (int w = 0; w < (wROI*vectorSize); w += wROI){
		if(x >= w && x < (w + wROI)){
			pow = (w/wROI);
		}
	}
	for (int h = 0; h < (hROI*vectorSize); h += hROI){
		if(y >= h && y < (h + hROI)){
			poh = (h/hROI);
		}
	}
	relevanceVector[(poh*vectorSize+pow)]++;
}

void relevanceDirectionToVector(int i, int angle){
	if((angle >= 0 && angle <= 22) || (angle > 338 && angle <=360)){
		// Right
		relevanceVector[i] = 2;
	}else if(angle > 22 && angle <= 68){
		// Top-Right
		relevanceVector[i] = 4;
	}else if(angle > 68 && angle <= 113){
		// Top
		relevanceVector[i] = 0;
	}else if(angle > 113 && angle <= 158){
		// Top-Left
		relevanceVector[i] = 5;
	}else if(angle > 158 && angle <= 203){
		// Left
		relevanceVector[i] = 3;
	}else if(angle > 203 && angle <= 248){
		// Bottom-left
		relevanceVector[i] = 7;
	}else if(angle > 248 && angle <= 293){
		// Bottom
		relevanceVector[i] = 1;
	}else if(angle > 293 && angle <= 338){
		// Bottom-Right
		relevanceVector[i] = 6;
	}
}

void printVector(short size){
	for(int i=0; i<size; i++){
		std::cout << relevanceVector[i] << ((i==(size-1))?" ":",");
	}
	std::cout << std::endl;
}

static void computeVectors( IplImage* img, IplImage* dst, short wROI, short hROI){
	if(DEBUG){
		std::cout << "-- VECTOR COMPUTING" << std::endl;		
	}
	double timestamp = (double)clock()/CLOCKS_PER_SEC; // get current time in seconds
	CvSize size = cvSize(img->width,img->height); // get current frame size 640x480
	int i, idx1 = last, idx2;
	CvSeq* seq;
	CvRect comp_rect;
	CvRect roi;
	double count;
	double angle;
	CvPoint center;
	double magnitude;
	CvScalar color;

	//--SURF CORNERS--
	if(DEBUG){
		std::cout << "--- SURF CORNERS" << std::endl;
	}
        color = CV_RGB(0,255,0);
        CvMemStorage* storage2 = cvCreateMemStorage(0);
        CvSURFParams params = cvSURFParams(SURF_THRESHOLD, 1);
        CvSeq *imageKeypoints = 0, *imageDescriptors = 0;
        cvExtractSURF( dst, 0, &imageKeypoints, &imageDescriptors, storage2, params );
        if(DEBUG){
			printf("Image Descriptors: %d\n", imageDescriptors->total);
		}
        for( int j = 0; j < imageKeypoints->total; j++ ){
            CvSURFPoint* r = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, j );
			center.x = cvRound(r->pt.x);
            center.y = cvRound(r->pt.y);
			if(DEBUG){
				printf("j: %d \t", j);               
				printf("total: %d \t", imageKeypoints->total);               
				printf("valor hessiano: %f \t", r->hessian);
				printf("x: %d \t", center.x);
				printf("y: %d \n", center.y);
			}
			// Agrego el Punto en donde es la region que nos interesa
			cvCircle( dst, center, cvRound(r->hessian*0.02), color, 3, CV_AA, 0 );
			// Lleno la matriz con los vectores
			relevancePointToVector(center.x, center.y, wROI, hROI, 5);
		}
	//--SURF CORNERS


	// calculate motion gradient orientation and valid orientation mask
	cvCalcMotionGradient( mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3 );
	
	// Compute Motion on 4x4 Cuadrants
	if(DEBUG){
		std::cout << "--- MOTION CUADRANTS" << std::endl;
	}
	i	 = 25;
	color = CV_RGB(255,0,0);
	magnitude = 30;
	for (int r = 0; r < size.height; r += hROI){
		for (int c = 0; c < size.width; c += wROI){
			comp_rect.x = c;
			comp_rect.y = r;
			comp_rect.width = (c + wROI > size.width) ? (size.width - c) : wROI;
			comp_rect.height = (r + hROI > size.height) ? (size.height - r) : hROI;

			cvSetImageROI( mhi, comp_rect );
			cvSetImageROI( orient, comp_rect );
			cvSetImageROI( mask, comp_rect );
			cvSetImageROI( silh, comp_rect );
			cvSetImageROI( img, comp_rect );

			// Process Motion
			angle = cvCalcGlobalOrientation( orient, mask, mhi, timestamp, MHI_DURATION);
			angle = 360.0 - angle;  // adjust for images with top-left origin
			count = cvNorm( silh, 0, CV_L1, 0 ); // calculate number of points within silhouette ROI
			roi = cvGetImageROI(mhi);
			center = cvPoint( (comp_rect.x + comp_rect.width/2),
					  (comp_rect.y + comp_rect.height/2) );
			cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
			cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
			cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );	
			
			if(DEBUG){
				std::cout << "Motion " << i << " -> x: " << roi.x << " y: " << roi.y << " count: " << count << " angle: " << angle << std::endl; // print the roi
			}
			cvResetImageROI( mhi );
			cvResetImageROI( orient );
			cvResetImageROI( mask );
			cvResetImageROI( silh );
			cvResetImageROI(img);
			relevanceDirectionToVector(i, angle);
			++i;
		}
	}

	// Compute Global Motion
	if(DEBUG){
		std::cout << "--- MOTION GLOBAL" << std::endl;
	}
	comp_rect = cvRect( 0, 0, size.width, size.height );
	color = CV_RGB(255,255,255);
	magnitude = 100;
	angle = cvCalcGlobalOrientation( orient, mask, mhi, timestamp, MHI_DURATION);
	angle = 360.0 - angle;  // adjust for images with top-left origin
	count = cvNorm( silh, 0, CV_L1, 0 ); // calculate number of points within silhouette ROI
	roi = cvGetImageROI(mhi);
	center = cvPoint( (comp_rect.x + comp_rect.width/2),
			  (comp_rect.y + comp_rect.height/2) );
	cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
	cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
	cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );
	if(DEBUG){
		std::cout << "Motion Main-> x: " << roi.x << " y: " << roi.y << " count: " << count << std::endl; // print the roi
	}
	relevanceDirectionToVector(50, angle);
}

// parameters:
//  img - input video frame
//  dst - resultant motion picture
//  args - optional parameters
static void  update_mhi( IplImage* img, IplImage* dst, int diff_threshold, int frameCount){
	if(DEBUG){
		std::cout << "- UPDATING_MHI" << std::endl;
	}
	double timestamp = (double)clock()/CLOCKS_PER_SEC; // get current time in seconds
	CvSize size = cvSize(img->width,img->height); // get current frame size
	int i, idx1 = last, idx2;
	CvSeq* seq;
	CvRect comp_rect;
	CvRect roi;
	double count;
	double angle;
	CvPoint center;
	double magnitude;
	CvScalar color;

	// Allocate images at the beginning or reallocate them if the frame size is changed
	if( !mhi || mhi->width != size.width || mhi->height != size.height ) {
		if( buf == 0 ) {
			buf = (IplImage**)malloc(N*sizeof(buf[0]));
			memset( buf, 0, N*sizeof(buf[0]));
		}

		for( i = 0; i < N; i++ ) {
			cvReleaseImage( &buf[i] );
			buf[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
			cvZero( buf[i] );
		}
		cvReleaseImage( &mhi );
		cvReleaseImage( &orient );
		cvReleaseImage( &segmask );
		cvReleaseImage( &mask );

		mhi = cvCreateImage( size, IPL_DEPTH_32F, 1 );
		cvZero( mhi ); // clear MHI at the beginning
		orient = cvCreateImage( size, IPL_DEPTH_32F, 1 );
		segmask = cvCreateImage( size, IPL_DEPTH_32F, 1 );
		mask = cvCreateImage( size, IPL_DEPTH_8U, 1 );
	}

	cvCvtColor( img, buf[last], CV_BGR2GRAY ); // convert frame to grayscale
	idx2 = (last + 1) % N; // index of (last - (N-1))th frame
	last = idx2;

	silh = buf[idx2];
	cvAbsDiff( buf[idx1], buf[idx2], silh ); // get difference between frames

	cvThreshold( silh, silh, diff_threshold, 255, CV_THRESH_BINARY); // and threshold it
	cvUpdateMotionHistory( silh, mhi, timestamp, MHI_DURATION ); // update MHI

	// convert MHI to blue 8u image
	cvCvtScale( mhi, mask, 255./MHI_DURATION, (MHI_DURATION - timestamp)*255./MHI_DURATION );
	cvZero( dst );
	cvMerge( mask, 0, 0, 0, dst );
}


int main(int argc, char** argv){
	IplImage* motion = 0;
	CvCapture* capture = 0;
	short frameCount = 0;

    if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
        capture = cvCaptureFromCAM( argc == 2 ? argv[1][0] - '0' : 0 );
    else if( argc == 2 )
        capture = cvCaptureFromFile( argv[1] );

	if( capture ){
		double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
		std::cout << "FPS : " <<  fps << std::endl;
		cvNamedWindow( "Motion", 1 );
		for(;;){
			IplImage* image = cvQueryFrame( capture );
			if( !image ){
				break;
			}
			if( !motion ){
                motion = cvCreateImage( cvSize(image->width,image->height), 8, 3 );
                cvZero( motion );
                motion->origin = image->origin;
            }
			++frameCount;
			if(frameCount > 7 || 1){
				frameCount = 0;
	    		initMatrix(relevanceVector,51);
				update_mhi( image, motion, MOTION_HISTORY_SENSITIVITY, frameCount);
	    		computeVectors(image, motion, 128, 96);
			}
			cvShowImage( "Motion", motion );
			if( cvWaitKey(10) >= 0 )
				break;
		}
		std::ostringstream oss;   // string as stream
		oss << argv[1] << ".jpg"; // write to string stream
		std::string file_name = oss.str(); // get string out of stream
		cvSaveImage(file_name.c_str(), motion);
		std::cout << "RV (" << argv[1] << ") : ";
		printVector(51);
		cvReleaseCapture( &capture );
		cvDestroyWindow( "Motion" );
	}
    return 0;
}

#ifdef _EiC
main(1,"motempl.c");
#endif
