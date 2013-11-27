#ifndef INPUT_HH_
# define INPUT_HH_

# include 	<string>
# include 	"frame.hh"

class 			Input
{
	public:
		virtual 					~Input(){}
		virtual bool 			open(int) = 0;
		virtual	bool 			open(const std::string &) = 0;
		virtual	bool  		isOpen() = 0;
		virtual	void 			release() = 0;
		virtual	Frame 		*getFrame() = 0;
};

#endif
