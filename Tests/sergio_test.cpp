#include 						<opencv2/opencv.hpp>
#include 						<iostream>

int 								main(int argc, char **argv)
{
	cv::VideoCapture 	cap(0); // open the default camera
	cv::Mat 					edges, frame; // cv:Mat is the n-dim matrix

	if (!cap.isOpened()) // Check if the camera is available
	{
		std::cerr << "Error opening default camera" << std::endl;
		return (-1);
	}
	cv::namedWindow("edges", 1);
	for(;;)
	{
		cap >> frame; // get a new frame for the camera
		cvtColor(frame, edges, CV_BGR2GRAY); // changes to grayscale
		GaussianBlur(edges, edges, cv::Size(7, 7), 1.5, 1.5);
		Canny(edges, edges, 0, 30, 3);
		cv::imshow("edges", edges);
		if (cv::waitKey(30) >= 0)
			break;
	}
	return (0);
}
