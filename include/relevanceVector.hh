#ifndef RELEVANCEVECTOR_HH_
# define RELEVANCEVECTOR_HH_

# include 	<vector>
# include 	"frame.hh"
# include 	"environment.hh"

class				RelevanceVector
{
	public:
		virtual 			~RelevanceVector(){}
		virtual void 	computeVectors(Frame *, Environment *) = 0;
		virtual void	*getRelevanceVector() = 0;
		virtual void 	setWROI(int) = 0;
		virtual void 	setHROI(int) = 0;
		virtual void 	setWSROI(int) = 0;
		virtual void 	setHSROI(int) = 0;
};

#endif
