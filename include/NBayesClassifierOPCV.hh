#ifndef NBAYESCLASSIFIEROPCV_HH_
# define NBAYESCLASSIFIEROPCV_HH_

# include 	<opencv2/opencv.hpp>
# include 	"classifierBehavior.hh"

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
		void 						printMat(CvMat *);
		virtual bool 		train();
		virtual float 	predict(RelevanceVector *);
};

#endif
