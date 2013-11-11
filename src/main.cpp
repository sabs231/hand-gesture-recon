#include 	<opencv2/highgui/highgui.hpp>
#include 	<iostream>
#include 	"exception.hh"

int 			main(int argc, char **argv)
{
	(void)argv;

	try
	{
		if (argc != 2)
		{
			throw (new ParameterException("./grecon <video file or device number>"));
			return (1);
		}
		std::cout << "good parameters" << std::endl;
	}
	catch (std::exception *e)
	{
		std::cerr << "grecon: " << e->what() << std::endl;
	}
	return (0);
}
