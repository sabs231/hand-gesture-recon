//
// main.m
// Background Substraction, 2 filters
// - Codebook Background model
// - Depth-cut Method
//
// Created by Guillermo Hernández on 21/08/2013.
//

#import "opencv/cv.h"
#import "opencv/highgui.h"

int main (int argc, const char * argv[]){
    
    IplImage* rawImage = 0;
    IplImage* bn       = NULL, *canny = NULL; // Detección de Borders
    int numeroFrames   = 0;
    int keyPressed     = 0;
    int ancho = 0, alto = 0, umbral1 = 1, umbral2 = 3, apertura = 3; // Detección de Borders
    
    // Fuente de Captura (Camara/Video)
    printf("Capture from camera\n");
    CvCapture* capture = 0;
    capture = cvCaptureFromCAM( 0 );
    
    // Fuentes de Display
    cvNamedWindow( "WithoutBorder", CV_WINDOW_AUTOSIZE );
    
    // Loop Principal
    while(TRUE){
        // Obtengo un Frame
        ++numeroFrames;
        rawImage = cvQueryFrame( capture );

        /*
         * Detección de bordes
         * Gracias a http://redstar.linaresdigital.com/robotica/filtro_canny.c
         */
        bn      = cvCreateImage(cvGetSize(rawImage), IPL_DEPTH_8U, 1);
        canny   = cvCreateImage(cvGetSize(rawImage), IPL_DEPTH_8U, 1);
        // Creamos una copia en blanco y negro del fotograma
        cvConvertImage(rawImage, bn, 0);
        // Aplicamos el filtro de detección de bordes
        cvCanny(bn, canny, umbral1, umbral2, apertura);
        // Ahora calculamos el área de la imagen donde queremos pegar la miniatura
        ancho = rawImage->width / 4;
        alto = rawImage->height / 4;
        cvSetImageROI(canny, cvRect(rawImage->width - (ancho * 1.1) , rawImage->height - (alto * 1.1), ancho, alto));
        // Ahora volcamos el contenido del fotograma a la zona deseada de la copia
        cvResize(bn, canny, CV_INTER_LINEAR);
        // Si no deshacemos la región de interés sólo veremos la zona recién copiada
        cvResetImageROI(canny);
        
        
        // Mostramos la imagen escalada
        cvShowImage("WithoutBorder", canny);
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