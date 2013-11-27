#ifndef FRAMEIMAGE_HH_
# define FRAMEIMAGE_HH_

class 	FrameImage
{
	public:
		virtual 			~FrameImage(){}
		virtual int 	getWidth() const = 0;
		virtual int 	getHeight() const = 0;
		virtual int 	getOrigin() const = 0; 
		virtual void 	setWidth(int) = 0;
		virtual void 	setHeight(int) = 0;
		virtual	void 	setOrigin(int) = 0;
};

#endif
