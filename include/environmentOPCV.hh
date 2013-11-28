#ifndef ENVIRONMENTOPCV_HH_
# define ENVIRONMENTOPCV_HH_

# include 	<opencv2/opencv.hpp>
# include 	"environment.hh"

class 			EnvironmentOPCV : public Environment
{
	private:
		IplImage 									*_silh;
		IplImage 									*_mhi;
		IplImage 									*_orient;
		IplImage 									*_mask;
		IplImage 									*_segmask;
		int 											_last;
	public:
		EnvironmentOPCV();
		~EnvironmentOPCV();
		virtual void 							*getSilh();
		virtual void 							*getMHI();
		virtual void 							*getOrient();
		virtual void 							*getMask();
		virtual void 							*getSegmask();
		virtual int 							getLast();
		virtual void 							setSilh(void *);
		virtual void 							setMHI(void *);
		virtual void 							setOrient(void *);
		virtual void 							setMask(void *);
		virtual void 							setSegmask(void *);
		virtual void 							setLast(int);
};

# endif
