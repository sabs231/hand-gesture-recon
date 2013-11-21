#ifndef NBAYESCLASSIFIEROPCV_HH_
# define NBAYESCLASSIFIEROPCV_HH_

# include 	<opencv2/opencv.hpp>

class 			NBayesClassifierOPCV : public ClassifierBehavior
{
	private:
		CvNormalBayesClassifier 	*nBayesClass;
		CvMat 										*data;
		CvMat 										*responses;
		CvMat 										*trainResp;
		CvMat 										*trainData;
		bool 											readNumClassData(const char *, int);
	public:
		NBayesClassifierOPCV(const char *);
		~NBayesClassifierOPCV();
		virtual bool 		train();
		virtual float 	predict();
};

#endif
