#ifndef FILTER_HH_
# define FILTER_HH_

# include 	"filterBehavior.hh"

class 			FilterBehavior;
class 			Frame;

class 			Filter
{
	protected:
		FilterBehavior 	*_filter;
	public:
		Filter();
		~Filter();
		void 	setFilter(FilterBehavior *);
		void 	performFilter(Frame *, Frame *);
};

#endif
