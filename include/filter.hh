#ifndef FILTER_HH_
# define FILTER_HH_

# include 	"FilterBehavior.hh"

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
