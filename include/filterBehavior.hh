#ifndef FILTERBEHAVIOR_HH_
# define FILTERBEHAVIOR_HH_

# include 	"frameImage.hh"

class 			FilterBehavior
{
	public:
		virtual 				~FilterBehavior();
		virtual void 		doFilter(FrameImage *, FrameImage *) = 0;
};

#endif
