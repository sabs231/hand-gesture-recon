#ifndef MOTIONDETECT_HH_
# define MOTIONDETECT_HH_

# include 	"mhiBehavior.hh"

class 			MotionDetect
{
	protected:
		MHIBehavior 	*_mhi;
	public:
		MotionDetect();
		~MotionDetect();
		void 		setMHIBehavior(MHIBehavior *);
		void 		updateMHI(Frame *, Frame *, Environment *);
};

#endif
