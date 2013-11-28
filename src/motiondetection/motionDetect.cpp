#include 	"motionDetect.hh"

MotionDetect::MotionDetect()
{
	this->_mhi = NULL;
}

MotionDetect::~MotionDetect()
{
}

void 		MotionDetect::setMHIBehavior(MHIBehavior *mhi)
{
	this->_mhi = mhi;
}

void 		MotionDetect::updateMHI(Frame *src, Frame *dest, Environment *env)
{
	this->_mhi->update(src, dest, env);
}
