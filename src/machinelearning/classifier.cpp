#include 	"classifier.hh"

Classifier::Classifier()
{
	this->_classifiers = new std::map<std::string, ClassifierBehavior *>();
}

Classifier::~Classifier()
{
	if (this->_classifiers)
		delete (this->_classifiers);
}

void 	Classifier::setClassifier(std::string name, ClassifierBehavior *classifier)
{
	this->_classifiers->insert(std::pair<std::string, ClassifierBehavior *>(name, classifier));
}

void 	Classifier::performTrain(const std::string &name)
{
	(*this->_classifiers)[name]->train();
}

void 	Classifier::performPredict(const std::string &name, RelevanceVector *rv)
{
	(*this->_classifiers)[name]->predict(rv);
}
