#include 	"environmentOPCV.hh"

EnvironmentOPCV::EnvironmentOPCV()
{
	this->_silh = NULL;
	this->_mhi = NULL;
	this->_orient = NULL;
	this->_mask = NULL;
	this->_segmask = NULL;
	this->_last = 0;
}
		
EnvironmentOPCV::~EnvironmentOPCV()
{
}

void 							*EnvironmentOPCV::getSilh()
{
	return (reinterpret_cast<void *>(this->_silh));
}

void 							*EnvironmentOPCV::getMHI()
{
	return (reinterpret_cast<void *>(this->_mhi));
}

void 							*EnvironmentOPCV::getOrient()
{
	return (reinterpret_cast<void *>(this->_orient));
}

void 							*EnvironmentOPCV::getMask()
{
	return (reinterpret_cast<void *>(this->_mask));
}

void 							*EnvironmentOPCV::getSegmask()
{
	return (reinterpret_cast<void *>(this->_segmask));
}

int 							EnvironmentOPCV::getLast()
{
	return (this->_last);
}

void 							EnvironmentOPCV::setSilh(void *si)
{
	this->_silh = reinterpret_cast<IplImage *>(si);
}

void 							EnvironmentOPCV::setMHI(void *mhi)
{
	this->_mhi = reinterpret_cast<IplImage *>(mhi);
}

void 							EnvironmentOPCV::setOrient(void *ori)
{
	this->_orient = reinterpret_cast<IplImage *>(ori);
}

void 							EnvironmentOPCV::setMask(void *mask)
{
	this->_mask = reinterpret_cast<IplImage *>(mask);
}

void 							EnvironmentOPCV::setSegmask(void *segm)
{
	this->_segmask = reinterpret_cast<IplImage *>(segm);
}

void 							EnvironmentOPCV::setLast(int last)
{
	this->_last = last;
}
