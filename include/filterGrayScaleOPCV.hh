#ifndef FILTERGRAYSCALEOPCV_HH_
# define FILTERGRAYSCALEOPCV_HH_

# include 	"filterBehavior.hh"

class 			FilterGrayScaleOPCV : public FilterBehavior
{
	public:
		FilterGrayScaleOPCV();
		~FilterGrayScale();
		virtual void 	doFilter(Frame *, Frame *);
};

#endif
