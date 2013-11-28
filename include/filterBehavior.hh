#ifndef FILTERBEHAVIOR_HH_
# define FILTERBEHAVIOR_HH_

# include 	"frame.hh"

class 			Frame;

class 			FilterBehavior
{
	public:
		virtual 				~FilterBehavior(){}
		virtual void 		doFilter(Frame *, Frame *) = 0;
};

#endif
