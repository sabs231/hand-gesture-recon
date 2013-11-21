#ifndef CLASSIFIER_BEHAVIOR_HH_
# define CLASSIFIER_BEHAVIOR_HH_

class 	ClassifierBehavior
{
	public:
		virtual bool 		train() = 0;
		virtual float 						predict() = 0;
};

#endif
