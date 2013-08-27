#include 						<opencv2/opencv.hpp>
#include 						<iostream>

int 								main(int argc, char **argv)
{
	cv::VideoCapture 	cap(0);
	cv::Mat 					edges, frame;

	if (!cap.isOpened())
	{
		std::cerr << "Error opening default camera" << std::endl;
		return (-1);
	}
	cv::namedWindow("edges", 1);
	for(;;)
	{
		cap >> frame;
		cvtColor(frame, edges, CV_BGR2GRAY);
		GaussianBlur(edges, edges, cv::Size(7, 7), 1.5, 1.5);
		Canny(edges, edges, 0, 30, 3);
		cv::imshow("edges", edges);
		if (cv::waitKey(30) >= 0)
			break;
	}
	return (0);
}
