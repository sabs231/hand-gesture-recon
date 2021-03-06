#include 	"frameOPCV.hh"

FrameOPCV::FrameOPCV()
	: Frame()
{
	this->_image = NULL;
}

FrameOPCV::FrameOPCV(int width, int height, int depth, int channels)
	: Frame()
{
	this->_image = cvCreateImage(cvSize(width, height), depth, channels);
}

IplImage 			*FrameOPCV::getIplImage()
{
	return (this->_image);
}

void 					FrameOPCV::setIplImage(IplImage *img)
{
	this->_image = img;
}

FrameOPCV::~FrameOPCV()
{
	if (this->_image)
		delete (this->_image);
}

void 	*FrameOPCV::getImage()
{
	return (reinterpret_cast<void *>(this->_image));
}

void 	FrameOPCV::setImage(void *img)
{
	this->_image = reinterpret_cast<IplImage *>(img);
}
		
void 	FrameOPCV::showImage(const std::string & wName)
{
	cvShowImage(wName.c_str(), this->_image);
}

int 	FrameOPCV::getWidth() const
{
	if (this->_image)
		return (this->_image->width);
	return (0);
}

int 	FrameOPCV::getHeight() const
{
	if (this->_image)
		return (this->_image->height);
	return (0);
}

int 	FrameOPCV::getOrigin() const
{
	if (this->_image)
		return (this->_image->origin);
	return (0);
}

void 	FrameOPCV::setWidth(int w)
{
	if (this->_image)
		this->_image->width = w;
}

void 	FrameOPCV::setHeight(int h)
{
	if (this->_image)
		this->_image->height = h;
}

void 	FrameOPCV::setOrigin(int o)
{
	if (this->_image)
		this->_image->origin = o;
}
