#ifndef MHIOPCV_HH_
# define MHIOPCV_HH_

# include 	<opencv2/opencv.hpp>
# include 	"frame.hh"
# include 	"mhiBehavior.hh"

class 			MHIOPCV : public MHIBehavior
{
	private:
		int 			_diffThreshold;
		int 			_cyclicFrame;
		double 		_mhiDuration;
		IplImage 	**_buf;
	public:
		MHIOPCV();
		MHIOPCV(int);
		~MHIOPCV();
		int 					getDiffThreshold() const;
		int 					getFrameCount() const;
		int 					getCyclicFrame() const;
		double 				getMHIDuration() const;
		void 					setDiffThreshold(int);
		void 					setFrameCount(int);
		void 					setCyclicFrame(int);
		void 					setMHIDuration(double);
		virtual void 	update(Frame *, Frame *, Environment *);
};

#endif
