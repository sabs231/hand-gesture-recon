#ifndef CLASSIFIER_BEHAVIOR_HH_
# define CLASSIFIER_BEHAVIOR_HH_

#include 	"relevanceVector.hpp"

template 	<typename T>
class 		ClassifierBehavior
{
	public:
		virtual 				~ClassifierBehavior(){}
		virtual bool 		train() = 0;
		virtual float 	predict(RelevanceVector<T> *) = 0;
};

#endif
