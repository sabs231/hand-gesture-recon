#include <iostream>
#include <fstream>
#include <sstream>
#include "opencv/cv.h"
#include <opencv2/opencv.hpp>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <map>
#include <exception>
#define DEBUG 0
const short COMPUTE_FRAME_THRESHOLD = 0; // How many frames should pass to compute a vector
const double MOTION_HISTORY_SENSITIVITY = 50;
const double MHI_DURATION = 1; // Para Obtencion de datos : 7
const double MGO_DURATION = 1; // Para Obtencion de datos : 7

void fillVideoMap(std::multimap<char,std::string> *videoFileClass){
	std::ostringstream fileNameStream;
	/* Hola
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
	*/
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

double relevanceDirectionToVector(int angle){
	if((angle >= 0 && angle <= 22) || (angle > 338 && angle <=360)){
		// Right
		return 2.0;
	}else if(angle > 22 && angle <= 68){
		// Top-Right
		return 4.0;
	}else if(angle > 68 && angle <= 113){
		// Top
		return 0.0;
	}else if(angle > 113 && angle <= 158){
		// Top-Left
		return 5.0;
	}else if(angle > 158 && angle <= 203){
		// Left
		return 3.0;
	}else if(angle > 203 && angle <= 248){
		// Bottom-left
		return 7.0;
	}else if(angle > 248 && angle <= 293){
		// Bottom
		return 1.0;
	}else if(angle > 293 && angle <= 338){
		// Bottom-Right
		return 6.0;
	}
}

// parameters:
std::vector<double> computeVectors(IplImage** mhi, IplImage* dst, short wROI, short hROI, short wSROI, short hSROI){
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
	for (int r = hROI; r < size.height - hROI; r += hSROI){
		for (int c = wROI; c < size.width - wROI; c += wSROI){
			comp_rect.x = c;
			comp_rect.y = r;
			comp_rect.width = wSROI;
			comp_rect.height = hSROI;

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

static int read_num_class_data( const char* filename, int var_count, CvMat** data, CvMat** responses ){
    const int M = 1024;
    FILE* f = fopen( filename, "rt" );
    CvMemStorage* storage;
    CvSeq* seq;
    char buf[M+2];
    float* el_ptr;
    CvSeqReader reader;
    int i, j;

    if( !f )
        return 0;

    el_ptr = new float[var_count+1];
    storage = cvCreateMemStorage();
	seq = cvCreateSeq( 0, sizeof(*seq), (var_count+1)*sizeof(float), storage );

    for(;;)
    {
        char* ptr;
        if( !fgets( buf, M, f ) || !strchr( buf, ',' ) )
            break;
        el_ptr[0] = buf[0];
        ptr = buf+2;
        for( i = 1; i <= var_count; i++ )
        {
            int n = 0;
            sscanf( ptr, "%f%n", el_ptr + i, &n );
            ptr += n + 1;
        }
        if( i <= var_count )
            break;
        cvSeqPush( seq, el_ptr );
    }
    fclose(f);

    *data = cvCreateMat( seq->total, var_count, CV_32F );
    *responses = cvCreateMat( seq->total, 1, CV_32F );

    cvStartReadSeq( seq, &reader );

    for( i = 0; i < seq->total; i++ )
    {
        const float* sdata = (float*)reader.ptr + 1;
        float* ddata = data[0]->data.fl + var_count*i;
        float* dr = responses[0]->data.fl + i;

        for( j = 0; j < var_count; j++ )
            ddata[j] = sdata[j];
        *dr = sdata[-1];
        CV_NEXT_SEQ_ELEM( seq->elem_size, reader );
    }

    cvReleaseMemStorage( &storage );
    delete el_ptr;
    return 1;
}

static int build_nbayes_classifier( char* data_filename, CvNormalBayesClassifier **nbayes){
    const int var_count = 33;
    CvMat* data = 0;
    CvMat train_data;
    CvMat* responses;

    int ok = read_num_class_data( data_filename, 33, &data, &responses );
    int nsamples_all = 0;
    int i, j;
    double train_hr = 0, test_hr = 0;
    CvANN_MLP mlp;

    if( !ok ){
        printf( "No se pudo leer la información de entrenamiento %s\n", data_filename );
        return -1;
    }

    printf( "La base de datos %s está siendo cargada...\n", data_filename );
    nsamples_all = data->rows;

    printf( "Entrenando el clasificador...\n");
	// 1. unroll the responses
    cvGetRows( data, &train_data, 0, nsamples_all);
    // 2. train classifier
    CvMat* train_resp = cvCreateMat( nsamples_all, 1, CV_32FC1);
    for (int i = 0; i < nsamples_all; i++){
        train_resp->data.fl[i] = responses->data.fl[i];
	}
	*nbayes = new CvNormalBayesClassifier(&train_data, train_resp);

    cvReleaseMat( &train_resp );
    cvReleaseMat( &data );
    cvReleaseMat( &responses );
    return 0;
}

void classify(CvNormalBayesClassifier *nbayes, std::vector<double> relevanceVector){
	/// Probando el clasificador
	double relevance[33];
	int i = 0;
	for (std::vector<double>::iterator it = relevanceVector.begin() ; it != relevanceVector.end(); ++it){
		relevance[i] = (*it);
		i++;
	}
	
	CvMat sample = cvMat(1, 33, CV_32FC1, relevance);
	CvMat *result = cvCreateMat(1, 1, CV_32FC1);
	// Predict
	float prediccion = 0.0000;
	if(nbayes != NULL){
		//prediccion = nbayes->predict(&sample, result);
		//prediccion = nbayes->predict(&sample, 0);
		prediccion = (float) nbayes->predict(&sample);
	}
	// Imprimiendo el Valor
	printf("Classify = %f\n",prediccion);
	if(prediccion == 81){
		std::cout << "QUERER" << std::endl;
	}else if(prediccion == 77){
		std::cout << "MATRIMONIO" << std::endl;
	}else if(prediccion == 72){
		std::cout << "HOLA" << std::endl;
	}else{
		std::cout << "NADA" << std::endl;
	}

	// Liberando Memoria
	cvReleaseMat(&result);
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
	double 		fps	= 0;
	int			wROI = 0;
	int			hROI = 0;
	int			wSROI = 0;
	int			hSROI = 0;
	std::ofstream 	relevanceVectorFile;
	std::vector<double> relevanceTmp;
	CvSize		ImageSize;
	
	// What to DO
	short obtain = 0;
	if( argc == 2 && strlen(argv[1]) == 6){
		obtain = 1;
	}
	
	try{
	if(obtain){
		std::vector< std::vector<double> > relevanceVectors;
		std::ostringstream fileNameStream;
		// Mapa de Videos
		std::multimap<char,std::string> videoFileClass;
		fillVideoMap(&videoFileClass);
		/*
		videoFileClass.insert(std::pair<char,std::string>('Q',"querer01"));
		videoFileClass.insert(std::pair<char,std::string>('Q',"querer02"));
		videoFileClass.insert(std::pair<char,std::string>('H',"hola00"));
		videoFileClass.insert(std::pair<char,std::string>('H',"hola20"));
		videoFileClass.insert(std::pair<char,std::string>('H',"hola21"));
		videoFileClass.insert(std::pair<char,std::string>('M',"matrimonio20"));
		videoFileClass.insert(std::pair<char,std::string>('M',"matrimonio21"));
		*/
		// Iterar por cada uno de los videos y obtener su vector de Relevancia
		//cvNamedWindow( "Motion", 1 );
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
					if(!captionOriginalImage){
						break;
					}
					ImageSize = cvSize(captionOriginalImage->width, captionOriginalImage->height);
					if( !motion ){
						motion = cvCreateImage( cvSize(captionOriginalImage->width,captionOriginalImage->height), 8, 3);
						cvZero( motion );
						motion->origin = captionOriginalImage->origin;
					}
					++frameCount;
					if(frameCount > COMPUTE_FRAME_THRESHOLD){
						frameCount = 0;
						update_mhi( captionOriginalImage, motion, &mhi, &buf, &silh, &mask, MOTION_HISTORY_SENSITIVITY, &lastHistoryFrame);
						wROI = ImageSize.width/5;
						hROI = ImageSize.height/5;
						wSROI= (ImageSize.width-((ImageSize.width/5)*2))/4;
						hSROI= (ImageSize.height-((ImageSize.height/5)*2))/4;
						relevanceVectors.push_back(computeVectors(&mhi, motion, wROI, hROI, wSROI, hSROI));
					}
					//cvShowImage( "Motion", motion );
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
				// 3/4th
				relevanceTmp = relevanceVectors[sizeMiddle + (sizeMiddle/2)];
				relevanceVectorFile << (*it).first;
				for (std::vector<double>::iterator it = relevanceTmp.begin() ; it != relevanceTmp.end(); ++it){
						relevanceVectorFile << ", " << (*it);
				}
				// Clean Relevance Vectors
				relevanceVectorFile << std::endl;
				for (std::vector< std::vector<double> >::iterator it = relevanceVectors.begin() ; it != relevanceVectors.end(); ++it){
					(*it).clear();
				}
				relevanceVectors.clear();
				// Clean MHI
				cvReleaseImage( &mhi );
				cvReleaseImage( &mask );
				cvReleaseImage( &motion );
				lastHistoryFrame = 0;
				frameCount = 0;
				if(cvWaitKey(10) >= 0) break;
			}
		}
		//cvDestroyWindow( "Motion" );
		relevanceVectorFile.close();
	}else{
		// Info del clasificador
		CvNormalBayesClassifier *nbayes;
		char default_data_filename[] = "./trainData.txt";
		char* data_filename = default_data_filename;
		build_nbayes_classifier(data_filename, &nbayes);
		// Empieza a classificar
			cvNamedWindow( "Original", 1 );
			cvNamedWindow( "Motion", 1 );	
			capture = cvCaptureFromCAM(0);
			if(capture){
				fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
				std::cout << "FPS: " <<  fps << std::endl;
				while(true){
					captionOriginalImage = cvQueryFrame( capture );
					ImageSize = cvSize(captionOriginalImage->width, captionOriginalImage->height);
					if(!captionOriginalImage) break;
					if( !motion ){
						motion = cvCreateImage( cvSize(captionOriginalImage->width,captionOriginalImage->height), 8, 3);
						cvZero( motion );
						motion->origin = captionOriginalImage->origin;
					}
						update_mhi( captionOriginalImage, motion, &mhi, &buf, &silh, &mask, MOTION_HISTORY_SENSITIVITY, &lastHistoryFrame);
						wROI = ImageSize.width/5;
						hROI = ImageSize.height/5;
						wSROI= (ImageSize.width-((ImageSize.width/5)*2))/4;
						hSROI= (ImageSize.height-((ImageSize.height/5)*2))/4;
						relevanceTmp = computeVectors(&mhi, motion, wROI, hROI, wSROI, hSROI);
					++frameCount;
					if(frameCount > 14){
						frameCount = 0;
						classify(nbayes, relevanceTmp);
					}
					cvShowImage( "Original", captionOriginalImage );
					cvShowImage( "Motion", motion );
					if(cvWaitKey(10) >= 0) break;
				}
				cvReleaseCapture( &capture );
			}
		cvDestroyWindow( "Motion" );
		cvDestroyWindow( "Original" );
	}
	}catch(...){
		std::cout << "ERROR!"<< std::endl;
		return 0;
	}

	return 0;
}
