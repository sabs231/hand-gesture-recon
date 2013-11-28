#ifndef MHIBEHAVIOR_HH_
# define MHIBEHAVIOR_HH_

# include 	"frame.hh"
# include 	"environment.hh"

class 			MHIBehavior
{
	public:
		virtual 			~MHIBehavior(){}
		virtual void 	update(Frame *, Frame *, Environment *) = 0;
};

#endif
