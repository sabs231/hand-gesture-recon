#ifndef CLASSIFIER_BEHAVIOR_HH_
# define CLASSIFIER_BEHAVIOR_HH_

#include 	"relevanceVector.hh"

class 		ClassifierBehavior
{
	public:
		virtual 				~ClassifierBehavior(){}
		virtual bool 		train() = 0;
		virtual float 	predict(RelevanceVector *) = 0;
};

#endif
