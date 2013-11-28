#ifndef FILTERGRAYSCALEOPCV_HH_
# define FILTERGRAYSCALEOPCV_HH_

# include 	"filterBehavior.hh"

class 			FilterGrayScaleOPCV : public FilterBehavior
{
	public:
		FilterGrayScaleOPCV();
		~FilterGrayScaleOPCV();
		virtual void 	doFilter(Frame *, Frame *);
};

#endif
