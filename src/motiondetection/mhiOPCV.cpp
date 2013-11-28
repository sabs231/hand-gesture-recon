#include 	<cstdlib>
#include 	"mhiOPCV.hh"

MHIOPCV::MHIOPCV()
{
	this->_diffThreshold = 0;
	this->_cyclicFrame = 4;
	this->_mhiDuration = 1;
	this->_buf = NULL;
}

MHIOPCV::MHIOPCV(int diff)
{
	this->_diffThreshold = diff;
	this->_cyclicFrame = 4;
	this->_mhiDuration = 1;
	this->_buf = NULL;
}

MHIOPCV::~MHIOPCV()
{
}

int 	MHIOPCV::getDiffThreshold() const
{
	return (this->_diffThreshold);
}

int 		MHIOPCV::getCyclicFrame() const
{
	return (this->_cyclicFrame);
}

double 	MHIOPCV::getMHIDuration() const
{
	return (this->_mhiDuration);
}

void 	MHIOPCV::setDiffThreshold(int diff)
{
	this->_diffThreshold = diff;
}

void	MHIOPCV::setCyclicFrame(int cyclicF)
{
	this->_cyclicFrame = cyclicF;
}

void 	MHIOPCV::setMHIDuration(double duration)
{
	this->_mhiDuration = duration;
}

void 	MHIOPCV::update(Frame *src, Frame *dest, Environment *env)
{
	int 			idx1;
	int 			idx2;
	int 			last;
	double 		timeStamp;
	IplImage 	*mhi;
	IplImage 	*mask;
	IplImage 	*silh;
	CvSize 		size = cvSize(reinterpret_cast<IplImage *>(src->getImage())->width, 
			reinterpret_cast<IplImage *>(src->getImage())->height);

	timeStamp = (double)clock() / CLOCKS_PER_SEC;
	last = env->getLast();
	idx1 = last;
	silh = reinterpret_cast<IplImage *>(env->getSilh());
	mhi = reinterpret_cast<IplImage *>(env->getMHI());
	mask = reinterpret_cast<IplImage *>(env->getMask());
	if (!mhi || mhi->width != size.width || mhi->height != size.height)
	{
		if (!this->_buf)
		{
			this->_buf = (IplImage **)malloc(this->_cyclicFrame * sizeof(this->_buf[0]));
			memset(this->_buf, 0, (this->_cyclicFrame * sizeof(this->_buf[0])));
		}
		for (int i = 0; i < this->_cyclicFrame; i++)
		{
			cvReleaseImage(&this->_buf[i]);
			this->_buf[i] = cvCreateImage(size, IPL_DEPTH_8U, 1);
			cvZero(this->_buf[i]);
		}
		cvReleaseImage(&mhi);
		cvReleaseImage(&mask);
		mhi = cvCreateImage(size, IPL_DEPTH_32F, 1);
		cvZero(mhi);
		env->setMHI(mhi);
		mask = cvCreateImage(size, IPL_DEPTH_8U, 1);
		env->setMask(mask);
	}
	cvCvtColor(reinterpret_cast<IplImage *>(src->getImage()), this->_buf[last], CV_BGR2GRAY);
	idx2 = (last + 1) % this->_cyclicFrame;
	last = idx2;
	env->setLast(last);
	silh = this->_buf[idx2];
	env->setSilh(silh);
	silh = reinterpret_cast<IplImage *>(env->getSilh());
	cvAbsDiff(this->_buf[idx1], this->_buf[idx2], silh);
	env->setSilh(silh);
	silh = reinterpret_cast<IplImage *>(env->getSilh());
	cvThreshold(silh, silh, this->_diffThreshold, 255, CV_THRESH_BINARY);
	env->setSilh(silh);
	silh = reinterpret_cast<IplImage *>(env->getSilh());
	cvUpdateMotionHistory(silh, mhi, timeStamp, this->_mhiDuration);
	env->setMHI(mhi);
	mhi = reinterpret_cast<IplImage *>(env->getMHI());
	cvCvtScale(mhi, mask, 255./this->_mhiDuration, (this->_mhiDuration - timeStamp) * 255./this->_mhiDuration);
	env->setMask(mask);
	mask = reinterpret_cast<IplImage *>(env->getMask());
	env->setMHI(mhi);
	mhi = reinterpret_cast<IplImage *>(env->getMHI());
	cvZero(reinterpret_cast<IplImage *>(dest->getImage()));
	cvMerge(mask, 0, 0, 0, reinterpret_cast<IplImage *>(dest->getImage()));
	env->setMask(mask);
	mask = reinterpret_cast<IplImage *>(env->getMask());
}
