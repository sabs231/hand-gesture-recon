#ifndef ENVIRONMENT
# define ENVIRONMENT

class 	Environment
{
	public:
		virtual 	~Environment(){}
		virtual void 	*getSilh() = 0;
		virtual void	*getMHI() = 0;
		virtual void 	*getOrient() = 0;
		virtual void 	*getMask() = 0;
		virtual void 	*getSegmask() = 0;
		virtual int 	getLast() = 0;
		virtual void 	setSilh(void *) = 0;
		virtual void 	setMHI(void *) = 0;
		virtual void 	setOrient(void *) = 0;
		virtual void 	setMask(void *) = 0;
		virtual void 	setSegmask(void *) = 0;
		virtual void 	setLast(int) = 0;
};

# endif
