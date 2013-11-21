#ifndef CLASSIFIER_HH_
# define CLASSIFIER_HH_

# include 	<map>
# include 	<string>
# include 	"ClassifierBehavior.hh"

class 			Classifier
{
	protected:
		std::map<const std::string &, ClassifierBehavior *> 	*_classifiers;
	public:
		void 	setClassifier(const std::string &, ClassifierBehavior *);
		void 	performTrain(const std::string &);
		void 	performPredict(const std::strin &);
};

#endif
