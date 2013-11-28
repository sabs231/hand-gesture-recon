#include 	"frame.hh"

Frame::Frame()
{
	this->_filter = new std::map<const std::string &, Filter *>();
}

Frame::~Frame()
{
	if (this->_filter)
		delete (this->_filter);
}

void Frame::addFilter(const std::string & name, Filter *f)
{
	this->_filter->insert(std::pair<const std::string &, Filter *>(name, f));
}

void Frame::removeFilter(const std::string & name)
{
	this->_filter->erase(name);
}
