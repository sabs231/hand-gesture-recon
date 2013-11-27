#include 	"inputOPCV.hh"

InputOPCV::InputOPCV()
{
	this->_capture = NULL;
}

InputOPCV::~InputOPCV()
{
	cvReleaseCapture(&this->_capture);
}

bool 			InputOPCV::open(int device)
{
	this->_capture = cvCaptureFromCAM(device); 
	if (!this->_capture)
		return (false);
	return (true);
}

bool 			InputOPCV::open(const std::string & fileName)
{
	this->_capture = cvCaptureFromFile(fileName.c_str());
	if (!this->_capture)
		return (false);
	return (true);
}

bool  		InputOPCV::isOpen()
{
	if (!this->_capture)
		return (false);
	return (true);
}

void 			InputOPCV::release()
{
	cvReleaseCapture(&this->_capture);
}

Frame 		*InputOPCV::getFrame()
{
	IplImage 	*img;
	Frame 		*nextFrame;

	img = cvQueryFrame(this->_capture);
	if (img)
	{
		nextFrame = new FrameOPCV(img->width, img->height, img->depth, img->nChannels);
		return (nextFrame);
	}
	return (NULL);
}
