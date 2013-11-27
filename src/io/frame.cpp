#include 	"frame.hh"

Frame::Frame()
{
	this->_filter = new std::vector<Filter *>();
}

Frame::~Frame()
{
	if (this->_filter)
		delete (this->_filter);
}

void Frame::addFilter(Filter *f)
{
	this->_filter->push_back(f);
}

void Frame::removeFilter(Filter *)
{
	//what!?
}
