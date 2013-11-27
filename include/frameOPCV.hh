#ifndef FRAMEOPCV_HH_
# define FRAMEOPCV_HH_

# include 	<opencv2/opencv.hpp>
# include 	"frame.hh"
# include 	"frameImage.hh"

class 			FrameOPCV : public Frame, public FrameImage
{
	private:
		IplImage 	*_image;
	public:
		FrameOPCV();
		FrameOPCV(int, int, int, int);
		~FrameImageOPCV();
		IplImage 			*getIplImage();
		void 					setIplImage(IplImage *);
		virtual void 	*getImage();
		virtual int 	getWidth() const;
		virtual int 	getHeight() const;
		virtual	int 	getOrigin() const;
		virtual	void 	setWidth(int);
		virtual	void 	setHeight(int);
		virtual	void 	setOrigin(int);
};

#endif
