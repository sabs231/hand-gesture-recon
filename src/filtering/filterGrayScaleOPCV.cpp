#include 	<opencv2/opencv.hpp>
#include 	"filterGrayScaleOPCV.hh"

FilterGrayScaleOPCV::FilterGrayScaleOPCV()
{
}

FilterGrayScaleOPCV::~FilterGrayScale()
{
}

void 	FilterGrayScaleOPCV::doFilter(Frame *src, Frame *dest)
{
	IplImage 	*mySrc;
	IplImage 	*myDest;

	mySrc = reinterpret_cast<IplImage *>(src->getImage());
	myDest = reinterpret_cast<IplImage *>(dest->getImage());
	cvCvtColot(mySrc, myDest, CV_BGR2GRAY);
}
