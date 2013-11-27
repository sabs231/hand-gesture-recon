#include 	<opencv2/opencv.hpp>
#include 	"filterGrayScaleOPCV.hh"

FilterGrayScaleOPCV::FilterGrayScaleOPCV()
{
}

FilterGrayScaleOPCV::~FilterGrayScale()
{
}

void 	FilterGrayScaleOPCV::doFilter(FrameImage *src, FrameImage *dest)
{
	cvCvtColot(src->getIplImage(), dest->getIplImage(), CV_BGR2GRAY);
}
