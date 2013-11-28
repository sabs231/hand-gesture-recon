#include 	"frame.hh"

Frame::Frame()
{
	this->_filter = new std::map<std::string, Filter *>();
}

Frame::~Frame()
{
	if (this->_filter)
		delete (this->_filter);
}

void Frame::addFilter(std::string name, Filter *f)
{
	this->_filter->insert(std::pair<std::string, Filter *>(name, f));
}

void Frame::removeFilter(std::string name)
{
	this->_filter->erase(name);
}
