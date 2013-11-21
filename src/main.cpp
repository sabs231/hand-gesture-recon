#include 				<opencv2/highgui/highgui.hpp>
#include 				<iostream>
#include 				"exception.hh"

using namespace cv;

int 						main(int argc, char **argv)
{
	try
	{
		if (argc != 3)
		{
			throw (new ParameterException("./grecon -v [video file name] | -d [device number]"));
			return (1);
		}
		std::string arg = argv[1];
		VideoCapture capture(arg);
	}
	catch (std::exception *e)
	{
		std::cerr << "grecon: " << e->what() << std::endl;
	}
	return (0);
}
