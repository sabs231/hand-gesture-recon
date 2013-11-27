#ifndef INPUTOPCV_HH_
# define INPUTOPCV_HH_

# include 	<opencv2/opencv.hpp>
#	include 	"input.hh"

class 			InputOPCV : public Input
{
	private:
		CvCapture 	*_capture;
	public:
		InputOPCV();
		~InputOPCV();
		virtual bool 			open(int);
		virtual bool 			open(const std::string &);
		virtual bool  		isOpen();
		virtual void 			release();
		virtual Frame 		*getFrame();
};

#endif
