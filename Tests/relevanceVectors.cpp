#include <iostream>
#include "opencv/cv.h"
#include <opencv2/opencv.hpp>
#include <time.h>
#include <stdio.h>
#include <ctype.h>

// various tracking parameters (in seconds)
const double MHI_DURATION = 1;
const double MAX_TIME_DELTA = 0.5;
const double MIN_TIME_DELTA = 0.05;
const double MOTION_HISTORY_SENSITIVITY = 45;//35
const double MOTION_VECTOR_SENSITIVITY = 150;//100
const double SURF_THRESHOLD = 600;//500
const double COMPUTE_FRAME_THRESHOLF = 1;// How many frames should pass to compute a vector
// number of cyclic frame buffer used for motion detection (should, probably, depend on FPS)
const int N = 4;

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
/* Direction
	0 top
	1 bottom
	2 right
	3 left
	4 top-right
	5 top-left
	6 bottom-right
	7 bottom-left
*/
int direction[5][5];
int magnitude[5][5];
int relevance[5][5];

void initMatrix(int matrix[5][5], short size){
	for(int i=0; i<size; i++)
		for(int j=0; j<size; j++)
			matrix[i][j]=0;

}

void recognition(){
	// Convert Vectors
}

static void computeVectors( IplImage* img, IplImage* dst, short wROI, short hROI){
	//std::cout << "-- VECTOR COMPUTING" << std::endl;
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
//	std::cout << "--- SURF CORNERS" << std::endl;
        color = CV_RGB(0,255,0);
        CvMemStorage* storage2 = cvCreateMemStorage(0);
        CvSURFParams params = cvSURFParams(SURF_THRESHOLD, 1);
        CvSeq *imageKeypoints = 0, *imageDescriptors = 0;
        cvExtractSURF( dst, 0, &imageKeypoints, &imageDescriptors, storage2, params );
        //printf("Image Descriptors: %d\n", imageDescriptors->total);
        for( int j = 0; j < imageKeypoints->total; j++ ){
            CvSURFPoint* r = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, j );
            //printf("j: %d \t", j);               
            //printf("total: %d \t", imageKeypoints->total);               
            //printf("valor hessiano: %f \t", r->hessian);
            center.x = cvRound(r->pt.x);
            center.y = cvRound(r->pt.y);
            //printf("x: %d \t", center.x);
            //printf("y: %d \n", center.y);
		// Agrego el Punto en donde es la region que nos interesa
            	cvCircle( dst, center, cvRound(r->hessian*0.02), color, 3, CV_AA, 0 );
		// Lleno la matriz con los vectores
		relevance[center.x/wROI][center.y/hROI]++;
        }
	//--SURF CORNERS


	// calculate motion gradient orientation and valid orientation mask
	cvCalcMotionGradient( mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3 );
	
	// Compute Motion on 4x4 Cuadrants
	//std::cout << "--- MOTION CUADRANTS" << std::endl;
	i	 = 0;
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

			//std::cout << "Motion " << i << " -> x: " << roi.x << " y: " << roi.y << " count: " << count << std::endl; // print the roi

			cvResetImageROI( mhi );
			cvResetImageROI( orient );
			cvResetImageROI( mask );
			cvResetImageROI( silh );
			cvResetImageROI(img);
			++i;
		}
	}

	// Compute Global Motion
	//std::cout << "--- MOTION GLOBAL" << std::endl;
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
	//std::cout << "Motion Main-> x: " << roi.x << " y: " << roi.y << " count: " << count << std::endl; // print the roi


	/*
	// segment motion: get sequence of motion components
	// segmask is marked motion components map. It is not used further
	// iterate through the motion components,
	// One more iteration (i == -1) corresponds to the whole image (global motion)
	seq = cvSegmentMotion( mhi, segmask, storage, timestamp, MAX_TIME_DELTA );
	if( !storage )
        	storage = cvCreateMemStorage(0);
	else
        	cvClearMemStorage(storage);
	for( i = -1; i < seq->total; i++ ) {
		if( i < 0 ) { // case of the whole image
		    comp_rect = cvRect( 0, 0, size.width, size.height );
		    color = CV_RGB(255,255,255);
		    magnitude = 100;
		}else { // i-th motion component
		    comp_rect = ((CvConnectedComp*)cvGetSeqElem( seq, i ))->rect;
		    if( comp_rect.width + comp_rect.height < MOTION_VECTOR_SENSITIVITY ) // reject very small components
			continue;
		    color = CV_RGB(255,0,0);
		    magnitude = 30;
		}

		// select component ROI
		cvSetImageROI( silh, comp_rect );
		cvSetImageROI( mhi, comp_rect );
		cvSetImageROI( orient, comp_rect );
		cvSetImageROI( mask, comp_rect );

		// calculate orientation
		angle = cvCalcGlobalOrientation( orient, mask, mhi, timestamp, MHI_DURATION);
		angle = 360.0 - angle;  // adjust for images with top-left origin

		count = cvNorm( silh, 0, CV_L1, 0 ); // calculate number of points within silhouette ROI

		roi = cvGetImageROI(mhi);
		//std::cout << "x: " << roi.x << " y: " << roi.y << " width: " << roi.width << " height: " << roi.height << std::endl; // print the roi

		cvResetImageROI( mhi );
		cvResetImageROI( orient );
		cvResetImageROI( mask );
		cvResetImageROI( silh );

		// check for the case of little motion
		if( count < comp_rect.width*comp_rect.height * 0.05 )
		    continue;

		// draw a clock with arrow indicating the direction
		center = cvPoint( (comp_rect.x + comp_rect.width/2),
				  (comp_rect.y + comp_rect.height/2) );

		cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
		cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
		cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );	
	}
	*/


}

// parameters:
//  img - input video frame
//  dst - resultant motion picture
//  args - optional parameters
static void  update_mhi( IplImage* img, IplImage* dst, int diff_threshold, int frameCount){
	std::cout << "- UPDATING_MHI" << std::endl;
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
	cvShowImage("hey apple", silh);
	cvUpdateMotionHistory( silh, mhi, timestamp, MHI_DURATION ); // update MHI

	// convert MHI to blue 8u image
	cvCvtScale( mhi, mask, 255./MHI_DURATION, (MHI_DURATION - timestamp)*255./MHI_DURATION );
	cvZero( dst );
	cvMerge( mask, 0, 0, 0, dst );
}


int main(int argc, char** argv){
	IplImage* motion = 0;
	CvCapture* capture = 0;
	short frameCount = 1;


    if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
        capture = cvCaptureFromCAM( argc == 2 ? argv[1][0] - '0' : 0 );
    else if( argc == 2 )
        capture = cvCaptureFromFile( argv[1] );

    if( capture )
    {
        cvNamedWindow( "Motion", 1 );

        for(;;)
        {
            IplImage* image = cvQueryFrame( capture );
            if( !image )
                break;

            if( !motion )
            {
                motion = cvCreateImage( cvSize(image->width,image->height), 8, 3 );
                cvZero( motion );
                motion->origin = image->origin;
            }
	    initMatrix(direction,5);
	    initMatrix(magnitude,5);
	    initMatrix(relevance,5);
            update_mhi( image, motion, MOTION_HISTORY_SENSITIVITY, frameCount);
	    computeVectors(image, motion, 128, 96);
	    if(frameCount > COMPUTE_FRAME_THRESHOLF){
		recognition();
		frameCount = 1;
	    }else{
		++frameCount;
	    }
            //cvShowImage( "Motion", motion );
            if( cvWaitKey(10) >= 0 )
                break;
        }
        cvReleaseCapture( &capture );
        cvDestroyWindow( "Motion" );
    }

    return 0;
}

#ifdef _EiC
main(1,"motempl.c");
#endif
