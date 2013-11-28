#include 	"environmentOPCV.hh"

Environment::Environment()
{
	this->_silh = NULL;
	this->_mhi = NULL;
	this->_orient = NULL;
	this->_mask = NULL;
	this->_segmask = NULL;
	this->_last = 0;
}
		
static EnvironmentOPCV *	EnvironmentOPCV::getInstance()
{
	if (!this->_uniqueInstance)
		this->_uniqueInstance = new EnvironmentOPCV();
	return (this->_uniqueInstance);
}

EnvironmentOPCV::~EnvironmentOPCV()
{
}

void 							*EnvironmentOPCV::getSilh()
{
	return (this->_silh);
}

void 							*EnvironmentOPCV::getMHI()
{
	return (this->_mhi);
}

void 							*EnvironmentOPCV::getOrient()
{
	return (this->_orient);
}

void 							*EnvironmentOPCV::getMask()
{
	return (this->_mask);
}

void 							*EnvironmentOPCV::getSegmask()
{
	return (this->_segmask);
}

int 							EnvironmentOPCV::getLast()
{
	return (this->_last);
}

void 							EnvironmnetOPCV::setSilh(void *si)
{
	this->_silh = si;
}

void 							EnvironmnetOPCV::setMHI(void *mhi)
{
	this->_mhi = mhi;
}

void 							EnvironmnetOPCV::setOrient(void *ori)
{
	this->_orient = ori;
}

void 							EnvironmnetOPCV::setMask(void *mask)
{
	this->_mask = mask;
}

void 							EnvironmnetOPCV::setSegmask(void *segm)
{
	this->_segmask = segm;
}

void 							EnvironmentOPCV::setLast(int last)
{
	this->_last = last;
}
