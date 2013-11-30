#ifndef CLASSIFIER_HH_
# define CLASSIFIER_HH_

# include 	<map>
# include 	<string>
# include 	"classifierBehavior.hh"

class 			Classifier
{
	protected:
		std::map<std::string, ClassifierBehavior *> 	*_classifiers;
	public:
		Classifier();
		~Classifier();
		void 	setClassifier(std::string, ClassifierBehavior *);
		void 	performTrain(const std::string &);
		void 	performPredict(const std::string &, RelevanceVector *);
};

#endif
