#include 				<cstdio>
#include 				<cstring>
#include 				<iostream>
#include 				"NBayesClassifierOPCV.hh"

bool 						NBayesClassifierOPCV::readNumClassData(const char *fileName, int varCount)
{
	FILE 					*f = fopen(fileName, "rt");
	const int 		M = 1024;
	const float 	*sdata;
	float 				*ddata;
	float 				*dr;
	char 					buf[M + 2];
	float 				*el_ptr;
	char 					*ptr;
	int 					n;
	CvSeqReader 	reader;
	CvMemStorage 	*storage;
	CvSeq 				*seq;

	if (!f)
		return (false);
	el_ptr = new float[varCount + 1];
	storage = cvCreateMemStorage();
	seq = cvCreateSeq(0, sizeof(*seq), sizeof(float) * (varCount + 1), storage);
	for (;;)
	{
		if (!fgets(buf, M, f) || !strchr(buf, ','))
			break;
		el_ptr[0] = buf[0];
		ptr = buf + 2;
		for (int i = 1; i <= varCount; i++)
		{
			n = 0;
			sscanf(ptr, "%f%n", el_ptr + i, &n);
			ptr = n + 1;
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
		ddata = this->data[0]->data.fl + varCount * i;
		dr = this->responses[0]->data.fl + i;
		for (int j = 0; j < varCount; j++)
			ddata[j] = sdata[j];
		*dr = sdata[-1];
		CV_NEXT_SEQ_ELEM(seq->elem_size, reader);
	}
	cvReleaseMemStorage(&storage);
	delete el_ptr;
	return (true);
}

NBayesClassifierOPCV::NBayesClassifierOPCV(const char *fileName)
{
	if (!this->readNumClassData(fileName, 51))
		throw (new FileReadException("Could not read the training data from" + std::string(fileName)));
	this->nBayesClass = new CvNormalBayesClassifier();
}

NBayesClassifierOPCV::~NBayesClassifierOPCV()
{
	cvReleaseMat(this->trainResp);
	cvReleaseMat(this->data);
	cvReleaseMat(this->responses);
	if (this->nBayesClass)
		delete this->nBayesClass;
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
					fprintf(stdout, "%8.3f", (float)cvGetReal2D(m, i, j))
				break;
			case CV_8U:
			case CV_16U:
				for (int j = 0; j < m->cols; j++)
					fprintf(stdout, "6d", (int)cvGetReal2D(m, i, j));
				break;
			default:
				break;
		}
	}
	std::cout << std::endl;
}

bool 		NBayesClassifierOPCV::train()
{
	int 	nSamplesAll = 0;

	nSamplesAll = this->data->rows;
	std::string << "Loading database..." << std::endl;
	std::string << "Trainning..." << std::endl;
	cvGetRows(this->data, this->trainData, 0, nSamplesAll); 
	this->trainResp = cvCreateMat(nSamplesAll, 1, CV_32C1);
	for (int i = 0; i < nSamplesAll; i++)
		this->trainResp->data.fl[i] = this->responses->data.fl[i];
	this->nBayesClass->train(this->trainData, this->trainResp);
	cvReleaseMat(this->trainResp);
	cvReleaseMat(this->data);
	cvReleaseMat(this->responses);
	return (true);
}

float 		NBayesClassifierOPCV::predict(RelevanceVector<float> *rV)
{
	float 	prediction;
	CvMat 	sample = cvMat(1, 51, CV_32FC1, rv->getRelevanceVector);

	prediction = 0.0000;
	if (this->nBayesClass)
	{
		prediction = (float) this->nBayesClass->predict(&sample);
		this->printMat(&sample);
	}
	std::cout << "Prediction: " << prediction << std::endl;
}
