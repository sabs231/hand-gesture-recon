#include 	<opencv2/opencv.hpp>
#include 	<cstdlib>
#include 	<ctime>

void 					my_init(CvSize size, IplImage ***buff, CvMemStorage **storage, int *buffLastIdx)
{
	*buff = (IplImage **)malloc(4 * sizeof(buff[0])); 	// 4-> Circular buffer size
	for (int i = 0; i < 4; i++)
	{
		(*buff)[i] = cvCreateImage(cvSize(size.width, size.height), IPL_DEPTH_8U, 1);
		cvZero((*buff)[i]);
	}
	*buffLastIdx = 0;
	*storage = cvCreateMemStorage(0);
}

void 					update_mhi(IplImage *src, IplImage *dest, int threshold, IplImage **mhi, IplImage **orient, IplImage **segmask, IplImage **mask, IplImage ***buff, int *buffLastIdx, int *buffOldestIdx, IplImage **diff)
{
	double 			timestamp = (double)clock() / CLOCKS_PER_SEC;
	CvSize 			size = cvSize(src->width, src->height);

	if ((!(*mhi)) || ((*mhi)->width != size.width || (*mhi)->height != size.height))
	{
		cvReleaseImage(mhi);
		cvReleaseImage(orient);
		cvReleaseImage(segmask);
		cvReleaseImage(mask);
		*mhi = cvCreateImage(size, IPL_DEPTH_32F, 1);
		cvZero(*mhi);
		*orient = cvCreateImage(size, IPL_DEPTH_32F, 1);
		*segmask = cvCreateImage(size, IPL_DEPTH_32F, 1);
		*mask = cvCreateImage(size, IPL_DEPTH_8U, 1);
	}
	cvCvtColor(src, (*buff)[*buffLastIdx], CV_BGR2GRAY);
	*diff = (*buff)[*buffOldestIdx];
	cvAbsDiff((*buff)[*buffLastIdx], (*buff)[*buffOldestIdx], *diff);
	cvThreshold(*diff, *diff, threshold, 1, CV_THRESH_BINARY);
	cvUpdateMotionHistory(*diff, *mhi, timestamp, 1);
	cvCvtScale(*mhi, *mask, (255. / 1), (1 - timestamp) * (255. / 1));
	cvZero(dest);
	cvMerge(*mask, 0, 0, 0, dest);
	(*buffLastIdx)++;
	if (*buffOldestIdx == *buffLastIdx)
		(*buffOldestIdx)++;
	if (*buffLastIdx > (4 - 1))
	{
		*buffLastIdx = 0;
		(*buffOldestIdx)++;
	}
	if (*buffOldestIdx > (4 - 1))
		*buffOldestIdx = 0;
}

int						main(int argc, char **argv)
{
	CvCapture 		*capture = 0;
	CvMemStorage 	*storage = 0;
	IplImage 			**buff;
	IplImage 			*frame = NULL;
	IplImage 			*image;
	IplImage 			*mhi = 0;
	IplImage 			*orient = 0;
	IplImage 			*mask = 0;
	IplImage 			*segmask = 0;
	IplImage			*diff;
	int 					buffLastIdx = 1;
	int 					buffOldestIdx = 1; 
	
	capture = cvCaptureFromCAM(-1);
	cvNamedWindow("video", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("motion", CV_WINDOW_AUTOSIZE);
	if (capture)
	{
		for (;;)
		{
			image = cvQueryFrame(capture);
			if (!image)
				break;
			if (!frame)
			{
				my_init(cvSize(image->width, image->height), &buff, &storage, &buffLastIdx);
				frame = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
				cvZero(frame);
				frame->origin = image->origin;
			}
			update_mhi(image, frame, 30, &mhi, &orient, &segmask, &mask, &buff, &buffLastIdx, &buffOldestIdx, &diff);
			cvShowImage("video", image);
			cvShowImage("motion", frame);
			if (cv::waitKey(10) >= 0)
				break;
		}
	}
	return (0);
}
