#include 	"filter.hh"

Filter::Filter()
{
	this->_filter = NULL;
}

Filter::~Filter()
{
	if (this->_filter)
		delete (this->_filter);
}

void 	Filter::setFilter(FilterBehavior *filter)
{
	this->_filter = filter;
}

void 	Filter::performFilter(Frame *src, Frame *dest)
{
	this->_filter->doFilter(src, dest);
}
