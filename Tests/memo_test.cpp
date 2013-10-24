//
// main.m
// Implementation of MHI and MHO
//
// Created by Guillermo Hernández on 21/08/2013.
//

#import "opencv/cv.h"
#import "opencv/highgui.h"

int main (int argc, const char * argv[]){
    
    IplImage* rawImage = NULL; // Original Image
    IplImage** buf     = NULL;
    IplImage* blur     = NULL; // Blur
    IplImage* bw       = NULL; // Black & White
    IplImage* canny    = NULL; // Detección de Border
    IplImage* silhouette  = NULL; // Silhouette to calc. Motion History Image
    IplImage* mhi      = NULL; // Motion History Image
    IplImage* mask      = NULL; // Motion History Gradient
    IplImage* orient      = NULL; // Motion History Gradient
    const double MHI_DURATION = 1;
    const double MAX_TIME_DELTA = 0.5;
    const double MIN_TIME_DELTA = 0.05;
    double timestamp   = 0.1;
    int numeroFrames   = 0;
    int keyPressed     = 0;
    int ancho = 0, alto = 0, umbral1 = 0, umbral2 = 30, apertura = 3; // Detección de Bordr
    int currBufIndex   = 0;
    CvSize size;            
    
    // Fuente de Captura (Camara/Video)
    printf("Capture from camera\n");
    CvCapture* capture = 0;
    capture = cvCaptureFromCAM( 0 );
    // capture = cvCaptureFromFile( argv[1] );
    
    // Fuentes de Display
    cvNamedWindow( "WithoutBorder", CV_WINDOW_AUTOSIZE );
    
    // Loop Principal
    while(TRUE){
        // Obtengo un Frame
        ++numeroFrames;
        rawImage = cvQueryFrame( capture );
	size     = cvGetSize(rawImage);

	/*
	 * To Black and White
	 */
        bw      = cvCreateImage(size, IPL_DEPTH_8U, 1);
        cvConvertImage(rawImage, bw, 0);

	/*
	 * Blur Images
	 */
        blur      = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvSmooth( bw, blur, CV_BLUR, 11, 11);
		
        /*
         * Detección de bordes
         * Gracias a http://redstar.linaresdigital.com/robotica/filtro_canny.c
         */
        canny   = cvCreateImage(size, IPL_DEPTH_8U, 1);
        cvCanny(blur, canny, umbral1, umbral2, apertura);

	/*
	 * MHI
	 */
        if(!mhi){
		cvReleaseImage( &mhi );
		mhi	= cvCreateImage(size, IPL_DEPTH_32F, 1);
		cvZero(mhi);
	}
	if(!silhouette){
		cvReleaseImage( &silhouette );
		silhouette	= cvCreateImage(size, IPL_DEPTH_8U, 1);
		cvZero(silhouette);
	}
        if( buf == NULL ) {
		buf = (IplImage**)malloc(2*sizeof(buf[0]));
		memset( buf, 0, 2*sizeof(buf[0]));
		for(int i = 0; i < 2; i++ ) {
			cvReleaseImage( &buf[i] );
			buf[i] = cvCreateImage(size, IPL_DEPTH_8U, 1 );
			cvZero( buf[i] );
		}
        }
	cvCopy(canny, buf[currBufIndex]);
	currBufIndex = (currBufIndex + 1) % 2; // Me voy moviendo entre el buffer
	cvAbsDiff( canny, buf[currBufIndex], silhouette );
	timestamp	= (double)clock()/CLOCKS_PER_SEC;
	cvUpdateMotionHistory( silhouette, mhi, timestamp, MHI_DURATION );

	/*
	 * MHG
	 */
        if(!mask || ! orient){
		cvReleaseImage( &mask );
		mask	= cvCreateImage(size, IPL_DEPTH_8U, 1);
		cvZero(mask);
		cvReleaseImage( &orient );
		orient	= cvCreateImage(size, IPL_DEPTH_8U, 1);
		cvZero(orient);

	}
	cvCalcMotionGradient( mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3 );

        cvShowImage("WithoutBorder", mhi);

        /*
	 * Display
	 * Ahora calculamos el área de la imagen donde queremos pegar la miniatur
	 *
        ancho = rawImage->width / 4;
        alto = rawImage->height / 4;
        cvSetImageROI(canny, cvRect(rawImage->width - (ancho * 1.1) , rawImage->height - (alto * 1.1), ancho, alto));
        // Ahora volcamos el contenido del fotograma a la zona deseada de la copia
        cvResize(bn, canny, CV_INTER_LINEAR);
        // Si no deshacemos la región de interés sólo veremos la zona recién copiada
        cvResetImageROI(canny); 
        // Mostramos la imagen escalada
        cvShowImage("WithoutBorder", canny);
	*/
       	
	 
	// Leo el teclado para posibles señales de salida, esperamos 100 ms
        keyPressed = cvWaitKey(10);
        if(keyPressed == 27 || keyPressed == 'q'){
            break;
        }
    }
    
    // Destruyo Elementos
    cvDestroyWindow( "WithoutBorder" );
    cvReleaseImage( &rawImage );
    return 0;
}
