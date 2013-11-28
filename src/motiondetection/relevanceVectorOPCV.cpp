#include 	<opencv2/opencv.hpp>
#include 	"relevanceVectorOPCV.hh"

RelevanceVectorOPCV::RelevanceVectorOPCV()
{
	this->_relevanceV = new std::vector<double>();
	this->_wROI = 0;
	this->_hROI = 0;
	this->_wSROI = 0;
	this->_hSROI = 0;
	this->_maxTimeDelta = 0.5;
	this->_minTimeDelta = 0.05;
}

RelevanceVectorOPCV::RelevanceVectorOPCV(double maxTime, double minTime)
{
	this->_relevanceV = new std::vector<double>();
	this->_wROI = 0;
	this->_hROI = 0;
	this->_wSROI = 0;
	this->_hSROI = 0;
	this->_maxTimeDelta = maxTime;
	this->_minTimeDelta = minTime;
}

RelevanceVectorOPCV::~RelevanceVectorOPCV()
{
	if (this->_relevanceV)
		delete (this->_relevanceV);
}

double	RelevanceVectorOPCV::getMaxTimeDelta() const
{
	return (this->_maxTimeDelta);
}

double 	RelevanceVectorOPCV::getMinTimeDelta() const
{
	return (this->_minTimeDelta);
}

void 		RelevanceVectorOPCV::setMaxTimeDelta(double maxTime)
{
	this->_maxTimeDelta = maxTime;
}

void 		RelevanceVectorOPCV::setMinTimeDelta(double minTime)
{
	this->_minTimeDelta = minTime;
}

void	RelevanceVectorOPCV::setWROI(int wROI)
{
	this->_wROI = wROI;
}

void 	RelevanceVectorOPCV::setHROI(int hROI)
{
	this->_hROI = hROI;
}

void 	RelevanceVectorOPCV::setWSROI(int wSROI)
{
	this->_wSROI = wSROI;
}

void 	RelevanceVectorOPCV::setHSROI(int hSROI)
{
	this->_hSROI = hSROI;
}

void	*RelevanceVectorOPCV::getRelevanceVector()
{
	return (reinterpret_cast<void *>(this->_relevanceV));
}

void 	RelevanceVectorOPCV::computeVectors(Frame *dest, Environment *env)
{
	int 			w;
	int 			h;
	int 			iWidth;
	int 			iHeight;
	double 		angle;
	double 		magnitude;
	double 		timeStamp;
	CvSize 		size;
	CvRect 		compRect;
	CvRect		roi;
	CvPoint		center;
	CvScalar 	color;
	IplImage 	*orient = 0;
	IplImage 	*mask = 0;

	timeStamp = (double) clock() / CLOCKS_PER_SEC;
	size = cvSize(reinterpret_cast<IplImage *>(dest->getImage())->width,
			reinterpret_cast<IplImage *>(dest->getImage())->height);
	orient = cvCreateImage(size, IPL_DEPTH_32F, 1);
	mask = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvCalcMotionGradient(reinterpret_cast<IplImage *>(env->getMHI()),
			mask, orient, this->_maxTimeDelta, this->_minTimeDelta, 3);
	compRect = cvRect(0, 0, size.width, size.height);
	color = CV_RGB(255, 255, 255);
	magnitude = 100;
	angle = cvCalcGlobalOrientation(orient, mask,
			reinterpret_cast<IplImage *>(env->getMHI()), timeStamp, 1); // 1->MGO_DURATION
	this->_relevanceV->push_back(angle);
	angle = 360.0 - angle;
	roi = cvGetImageROI(reinterpret_cast<IplImage *>(env->getMHI()));
	center = cvPoint((compRect.x + compRect.width / 2), (compRect.y + compRect.height / 2));
	cvCircle(reinterpret_cast<IplImage *>(dest->getImage()), center, cvRound(magnitude * 1.2),
			color, 3, CV_AA, 0);
	cvLine(reinterpret_cast<IplImage *>(dest->getImage()), center,
			cvPoint(cvRound(center.x + magnitude * cos(angle * CV_PI / 180)),
				cvRound(center.y - magnitude * sin(angle * CV_PI / 180))), color, 3, CV_AA, 0);
	iWidth = (reinterpret_cast<IplImage *>(dest->getImage())->width / this->_wROI);
	iHeight = (reinterpret_cast<IplImage *>(dest->getImage())->height / this->_hROI);
	w = 0;
	h = 1;
	color = CV_RGB(255, 0, 0);
	magnitude = 30;
	while (w < iWidth)
	{
		//TOP
		compRect.x = w * this->_wROI;
		compRect.y = 0;
		compRect.width = this->_wROI;
		compRect.height = this->_hROI;
		cvSetImageROI(reinterpret_cast<IplImage *>(env->getMHI()), compRect);
		cvSetImageROI(orient, compRect);
		cvSetImageROI(mask, compRect);
		angle = cvCalcGlobalOrientation(orient, mask, reinterpret_cast<IplImage *>(env->getMHI()),
				timeStamp, 1); // 1 -> MHI_DURATION
		this->_relevanceV->push_back(angle);
		angle = 360.0 - angle;
		roi = cvGetImageROI(reinterpret_cast<IplImage *>(env->getMHI()));
		center = cvPoint((compRect.x + compRect.width / 2), (compRect.y + compRect.height / 2));
		cvCircle(reinterpret_cast<IplImage *>(dest->getImage()), center, cvRound(magnitude * 1.2),
				color, 3, CV_AA, 0);
		cvLine(reinterpret_cast<IplImage *>(dest->getImage()), center,
				cvPoint(cvRound(center.x + magnitude * cos(angle * CV_PI / 180)),
					cvRound(center.y - magnitude * sin(angle * CV_PI / 180))), color, 3, CV_AA, 0);
		cvResetImageROI(reinterpret_cast<IplImage *>(env->getMHI()));
		cvResetImageROI(orient);
		cvResetImageROI(mask);
		//BOTTOM
		compRect.x = w * this->_wROI;
		compRect.y = reinterpret_cast<IplImage *>(dest->getImage())->height - this->_hROI;
		cvSetImageROI(reinterpret_cast<IplImage *>(env->getMHI()), compRect);
		cvSetImageROI(orient, compRect);
		cvSetImageROI(mask, compRect);
		angle = cvCalcGlobalOrientation(orient, mask, reinterpret_cast<IplImage *>(env->getMHI()),
				timeStamp, 1); // 1 -> MHI_DURATION
		this->_relevanceV->push_back(angle);
		angle = 360.0 - angle;
		roi = cvGetImageROI(reinterpret_cast<IplImage *>(env->getMHI()));
		center = cvPoint((compRect.x + compRect.width / 2), (compRect.y + compRect.height / 2));
		cvCircle(reinterpret_cast<IplImage *>(dest->getImage()), center, cvRound(magnitude * 1.2),
				color, 3, CV_AA, 0);
		cvLine(reinterpret_cast<IplImage *>(dest->getImage()), center,
				cvPoint(cvRound(center.x + magnitude * cos(angle * CV_PI / 180)),
					cvRound(center.y - magnitude * sin(angle * CV_PI / 180))), color, 3, CV_AA, 0);
		cvResetImageROI(reinterpret_cast<IplImage *>(env->getMHI()));
		cvResetImageROI(orient);
		cvResetImageROI(mask);
		w++;
	}
	while (h < (iHeight - 1))
	{
		//LEFT
		compRect.x = 0;
		compRect.y = h * this->_hROI;
		compRect.width = this->_wROI;
		compRect.height = this->_hROI;
		cvSetImageROI(reinterpret_cast<IplImage *>(env->getMHI()), compRect);
		cvSetImageROI(orient, compRect);
		cvSetImageROI(mask, compRect);
		angle = cvCalcGlobalOrientation(orient, mask, reinterpret_cast<IplImage *>(env->getMHI()),
				timeStamp, 1); // 1 -> MHI_DURATION
		this->_relevanceV->push_back(angle);
		angle = 360.0 - angle;
		roi = cvGetImageROI(reinterpret_cast<IplImage *>(env->getMHI()));
		center = cvPoint((compRect.x + compRect.width / 2), (compRect.y + compRect.height / 2));
		cvCircle(reinterpret_cast<IplImage *>(dest->getImage()), center, cvRound(magnitude * 1.2),
				color, 3, CV_AA, 0);
		cvLine(reinterpret_cast<IplImage *>(dest->getImage()), center,
				cvPoint(cvRound(center.x + magnitude * cos(angle * CV_PI / 180)),
					cvRound(center.y - magnitude * sin(angle * CV_PI / 180))), color, 3, CV_AA, 0);
		cvResetImageROI(reinterpret_cast<IplImage *>(env->getMHI()));
		cvResetImageROI(orient);
		cvResetImageROI(mask);
		//RIGHT
		compRect.x = reinterpret_cast<IplImage *>(dest->getImage())->width - this->_wROI;
		compRect.y = h * this->_hROI;
		cvSetImageROI(reinterpret_cast<IplImage *>(env->getMHI()), compRect);
		cvSetImageROI(orient, compRect);
		cvSetImageROI(mask, compRect);
		angle = cvCalcGlobalOrientation(orient, mask, reinterpret_cast<IplImage *>(env->getMHI()),
				timeStamp, 1); // 1 -> MHI_DURATION
		this->_relevanceV->push_back(angle);
		angle = 360.0 - angle;
		roi = cvGetImageROI(reinterpret_cast<IplImage *>(env->getMHI()));
		center = cvPoint((compRect.x + compRect.width / 2), (compRect.y + compRect.height / 2));
		cvCircle(reinterpret_cast<IplImage *>(dest->getImage()), center, cvRound(magnitude * 1.2),
				color, 3, CV_AA, 0);
		cvLine(reinterpret_cast<IplImage *>(dest->getImage()), center,
				cvPoint(cvRound(center.x + magnitude * cos(angle * CV_PI / 180)),
					cvRound(center.y - magnitude * sin(angle * CV_PI / 180))), color, 3, CV_AA, 0);
		cvResetImageROI(reinterpret_cast<IplImage *>(env->getMHI()));
		cvResetImageROI(orient);
		cvResetImageROI(mask);
		h++;
	}
	color = CV_RGB(255, 0, 0);
	magnitude = 20;
	for (int r = this->_hROI; r < size.height - this->_hROI; r += this->_hSROI)
	{
		for (int c = this->_wROI; c < size.width - this->_wROI; c += this->_wSROI)
		{
			compRect.x = c;
			compRect.y = r;
			compRect.width = this->_wROI;
			compRect.height = this->_hROI;
			cvSetImageROI(reinterpret_cast<IplImage *>(env->getMHI()), compRect);
			cvSetImageROI(orient, compRect);
			cvSetImageROI(mask, compRect);
			angle = cvCalcGlobalOrientation(orient, mask, reinterpret_cast<IplImage *>(env->getMHI()),
					timeStamp, 1); // 1 -> MHI_DURATION
			this->_relevanceV->push_back(angle);
			angle = 360.0 - angle;
			roi = cvGetImageROI(reinterpret_cast<IplImage *>(env->getMHI()));
			center = cvPoint((compRect.x + compRect.width / 2), (compRect.y + compRect.height / 2));
			cvCircle(reinterpret_cast<IplImage *>(dest->getImage()), center, cvRound(magnitude * 1.2),
					color, 3, CV_AA, 0);
			cvLine(reinterpret_cast<IplImage *>(dest->getImage()), center,
					cvPoint(cvRound(center.x + magnitude * cos(angle * CV_PI / 180)),
						cvRound(center.y - magnitude * sin(angle * CV_PI / 180))), color, 3, CV_AA, 0);
			cvResetImageROI(reinterpret_cast<IplImage *>(env->getMHI()));
			cvResetImageROI(orient);
			cvResetImageROI(mask);
		}
	}
}
