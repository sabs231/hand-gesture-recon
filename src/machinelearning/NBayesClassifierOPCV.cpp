#include 				<cstdio>
#include 				<cstring>
#include 				<iostream>
#include 				"nBayesClassifierOPCV.hh"
#include 				"exception.hh"

bool 						NBayesClassifierOPCV::readNumClassData(const std::string & fileName, int varCount)
{
	FILE 					*f = fopen(fileName.c_str(), "rt");
	const int 		M = 1024;
	const float 	*sdata;
	float 				*ddata;
	float 				*dr;
	char 					buf[M + 2];
	float 				*el_ptr;
	char 					*ptr;
	int 					n;
	int 					i;
	CvSeqReader 	reader;
	CvMemStorage 	*storage;
	CvSeq 				*seq;

	if (!f)
	{
		perror("Reason");
		return (false);
	}
	el_ptr = new float[varCount + 1];
	storage = cvCreateMemStorage();
	seq = cvCreateSeq(0, sizeof(*seq), sizeof(float) * (varCount + 1), storage);
	for (;;)
	{
		if (!fgets(buf, M, f) || !strchr(buf, ','))
			break;
		el_ptr[0] = buf[0];
		ptr = buf + 2;
		for (i = 1; i <= varCount; i++)
		{
			n = 0;
			sscanf(ptr, "%f%n", el_ptr + i, &n);
			ptr += n + 1;
		}
		if (i <= varCount)
			break;
		cvSeqPush(seq, el_ptr);
	}
	fclose(f);
	this->data = cvCreateMat(seq->total, varCount, CV_32F);
	this->responses = cvCreateMat(seq->total, 1, CV_32F);
	cvStartReadSeq(seq, &reader);
	for (int i = 0; i < seq->total; i++)
	{
		sdata = (float *)reader.ptr + 1;
		ddata = this->data->data.fl + varCount * i;
		dr = this->responses->data.fl + i;
		for (int j = 0; j < varCount; j++)
			ddata[j] = sdata[j];
		*dr = sdata[-1];
		CV_NEXT_SEQ_ELEM(seq->elem_size, reader);
	}
	cvReleaseMemStorage(&storage);
	delete (el_ptr);
	return (true);
}

NBayesClassifierOPCV::NBayesClassifierOPCV(const std::string & fileName)
{
	if (!this->readNumClassData(fileName, 33))
		throw (new FileReadException("Could not read the training data from " + std::string(fileName)));
	this->nBayesClass = new CvNormalBayesClassifier();
}

NBayesClassifierOPCV::~NBayesClassifierOPCV()
{
	if (this->nBayesClass)
		delete (this->nBayesClass);
}

void 		NBayesClassifierOPCV::printMat(CvMat *m)
{
	for (int i = 0; i < m->rows; i++)
	{
		std::cout << std::endl;
		switch (CV_MAT_DEPTH(m->type))
		{
			case CV_32F:
			case CV_64F:
				for (int j = 0; j < m->cols; j++)
					fprintf(stdout, "%8.3f", (float)cvGetReal2D(m, i, j));
				break;
			case CV_8U:
			case CV_16U:
				for (int j = 0; j < m->cols; j++)
					fprintf(stdout, "%6d", (int)cvGetReal2D(m, i, j));
				break;
			default:
				break;
		}
	}
	std::cout << std::endl;
}

bool 		NBayesClassifierOPCV::train()
{
	int 		nSamplesAll = 0;
	CvMat 	tData;

	nSamplesAll = this->data->rows;
	std::cout << "Loading database..." << std::endl;
	std::cout << "Trainning..." << std::endl;
	cvGetRows(this->data, &tData, 0, nSamplesAll); 
	this->trainResp = cvCreateMat(nSamplesAll, 1, CV_32FC1);
	for (int i = 0; i < nSamplesAll; i++)
		this->trainResp->data.fl[i] = this->responses->data.fl[i];
	this->trainData = &tData;
	this->nBayesClass->train(this->trainData, this->trainResp);
	cvReleaseMat(&this->trainResp);
	cvReleaseMat(&this->data);
	cvReleaseMat(&this->responses);
	std::cout << "The trainning has been completed" << std::endl;
	return (true);
}

float 		NBayesClassifierOPCV::predict(RelevanceVector *rv)
{
	int 														i;
	float 													prediction;
	double 													relevance[33];
	std::vector<double>::iterator 	it;
	std::vector<double> 						*vec;

	vec = reinterpret_cast<std::vector<double> *>(rv->getRelevanceVector());
	i = 0;
	for (it = vec->begin(); it != vec->end(); ++it)
	{
			relevance[i] = (*it);
			i++;
	}
	CvMat 	sample = cvMat(1, 33, CV_32FC1, relevance);
	prediction = 0.0000;
	if (this->nBayesClass)
		prediction = (float) this->nBayesClass->predict(&sample);
	std::cout << "Prediction: " << prediction << std::endl;
	if (prediction == 81)
		std::cout << "QUERER" << std::endl;
	else if (prediction == 77)
		std::cout << "MATRIMONIO" << std::endl;
	else if (prediction == 72)
		std::cout << "HOLA" << std::endl;
	else
		std::cout << "NADA" << std::endl;
	return (prediction);
}
