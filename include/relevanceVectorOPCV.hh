#ifndef RELEVANCEVECTOROPCV_HH_
# define RELEVANCEVECTOROPCV_HH_

# include 	"relevanceVector.hh"

class 			RelevanceVectorOPCV : public RelevanceVector
{
	private:
		std::vector<double> 	*_relevanceV;
		int 									_wROI;
		int 									_hROI;
		int 									_wSROI;
		int 									_hSROI;
		double 								_maxTimeDelta;
		double 								_minTimeDelta;
	public:
		RelevanceVectorOPCV();
		RelevanceVectorOPCV(double, double);
		~RelevanceVectorOPCV();
		double 				getMaxTimeDelta() const;
		double 				getMinTimeDelta() const;
		void 					setMaxTimeDelta(double);
		void 					setMinTimeDelta(double);
		virtual void 	setWROI(int);
		virtual void 	setHROI(int);
		virtual void 	setWSROI(int);
		virtual void 	setHSROI(int);
		virtual void 	computeVectors(Frame *, Environment *);
		virtual void	*getRelevanceVector();
};

#endif
