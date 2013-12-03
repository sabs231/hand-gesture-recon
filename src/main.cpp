#include 				<iostream>
#include 				"exception.hh"
#include 				"inputOPCV.hh"
#include 				"frameOPCV.hh"
#include 				"mhiOPCV.hh"
#include 				"environmentOPCV.hh"
#include 				"motionDetect.hh"
#include 				"relevanceVectorOPCV.hh"
#include 				"nBayesClassifierOPCV.hh"
#include 				"classifier.hh"

#include 				<opencv2/opencv.hpp>

int 						main(int argc, char **argv)
{
	short								frameCount;
	int 								width;
	int 								height;
	std::string					arg;
	Input 							*input;
	Frame 							*image;
	Frame 							*motion;
	MHIOPCV 						*myMHI;
	Environment					*env;
	MotionDetect 				*detection;
	RelevanceVector 		*rv;
	Classifier 					*classifier;
	ClassifierBehavior 	*nBayes;

	try
	{
		input = new InputOPCV();
		if (argc == 1)
		{
			if (!input->open(0))
			{
				std::cerr << "Camera not available" << std::endl;
				return (1);
			}
		}
		else if (argc == 2)
		{
			arg = argv[1];
			if (!input->open(arg))
			{
				std::cerr << "Video: " << arg << " not available" << std::endl;
				return (1);
			}
		}
		else
		{
			throw (new ParameterException("./grecon [video file name] | [without parameter gets the camera]"));
			return (1);
		}
		motion = NULL;
		width = 0;
		height = 0;
		frameCount = 0;
		myMHI = new MHIOPCV();
		env = new EnvironmentOPCV();
		detection = new MotionDetect();
		rv = new RelevanceVectorOPCV(0.5, 0.05);
		nBayes = new NBayesClassifierOPCV("./trainData/trainData.txt");
		classifier = new Classifier();
		detection->setMHIBehavior(myMHI);
		classifier->setClassifier("NBayes", nBayes);
		classifier->performTrain("NBayes");
		while (42) // answer of everything!
		{
			image = input->getFrame();
			if (!image)
				break;
			if (!motion)
			{
				width = reinterpret_cast<IplImage *>(image->getImage())->width;
				height = reinterpret_cast<IplImage *>(image->getImage())->height;
				motion = new FrameOPCV(width, height, 8, 3);
				cvZero(reinterpret_cast<IplImage *>(motion->getImage())); // this will change
				reinterpret_cast<IplImage *>(motion->getImage())->origin = reinterpret_cast<IplImage *>(image->getImage())->origin;
			}
			detection->updateMHI(image, motion, env);
			rv->setWROI(width / 5);
			rv->setHROI(height / 5);
			rv->setWSROI((width - ((width / 5) * 2)) / 4);
			rv->setHSROI((height - ((height / 5) * 2)) / 4);
			rv->computeVectors(motion, env);
			++frameCount;
			if (frameCount > 14)
			{
				frameCount = 0;
				classifier->performPredict("NBayes", rv);
			}
			image->showImage("original");
			motion->showImage("motion");
			if (cvWaitKey(10) >= 0)
				break;
		}
		cvDestroyWindow("motion");
		cvDestroyWindow("original");
	}
	catch (std::exception *e)
	{
		std::cerr << "grecon: " << e->what() << std::endl;
	}
	return (0);
}
