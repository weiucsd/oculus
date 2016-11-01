#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <math.h>
#include <Windows.h>
#include "svm.h"
using namespace std;

#define FRAME 40
#define HIST 100
//// Machine Learning class
class ML
{
	vector<vector<double>> centroids;
	vector<string> modelName;
	//// The models are saved in the order of 1-2, 1-3, 1-4, 1-5, 2-3, 2-4, 2-5, 3-4, 3-5, 4-5.
	//// 1: grab, 2: throw, 3: right_rotation, 4: left_rotation, 5: wave_hand
	vector<svm_model *> models;
	svm_node nodes[HIST+1];

public:
	ML()
	{
		//// Read cluster file
		ifstream file("cluster.txt");
		string line;

		while (getline(file, line))
		{
			stringstream lineStream(line);
			string cell;
			vector<double> centroid;
			for (int i = 0; i < 4; i++)
			{
				getline(lineStream, cell, ',');
				centroid.push_back(stod(cell));
			}
			centroids.push_back(centroid);
		}

		//// Load svm models
		for (int i = 1; i <= 4; i++)
		{
			for (int j = i + 1; j <= 5; j++)
			{
				modelName.push_back(to_string(i) + "-" + to_string(j) + ".model");
				models.push_back(svm_load_model(modelName.back().c_str()));
			}
		}
	}

	~ML(){}

	double CalcDistance(const double* x, const double* y, int dimNum)
	{
		double temp = 0;
		for (int d = 0; d < dimNum; d++)
		{
			temp += (x[d] - y[d]) * (x[d] - y[d]);
		}
		return temp;
	}

	int GetLabels(double angle[4])
	{
		int label = 0;
		//// Calculate labels
		double dist = CalcDistance(angle, &centroids[0][0], 4);
		for (unsigned int j = 1; j < centroids.size(); j++)
		{
			double temp = CalcDistance(angle, &centroids[j][0], 4);
			if (temp < dist)
			{
				dist = temp;
				label = j;
			}
		}
		return label;
	}

	void GetAngels(vector<Vector3f> frame, double angel[4])
	{
		// frame's form: palm, thumb, index, middle, ring, little
		// Substract palm from each finger:
		double f[15];
		for (int i = 0; i < 5; i++)
		{
			f[i * 3] = frame[i + 1].x - frame[0].x;
			f[i * 3 + 1] = frame[i + 1].y - frame[0].y;
			f[i * 3 + 2] = frame[i + 1].z - frame[0].z;
		}

		for (int i = 0; i < 4; i++)
		{
			angel[i] = acos((f[i * 3] * f[i * 3 + 3] + f[i * 3 + 1] * f[i * 3 + 4] + f[i * 3 + 2] * f[i * 3 + 5]) / sqrt(
				(f[i * 3] * f[i * 3] + f[i * 3 + 1] * f[i * 3 + 1] + f[i * 3 + 2] * f[i * 3 + 2]) *
				(f[i * 3 + 3] * f[i * 3 + 3] + f[i * 3 + 4] * f[i * 3 + 4] + f[i * 3 + 5] * f[i * 3 + 5])));
		}
	}

	void ReadHist(double* hist)
	{
		for (int i = 0; i < HIST; i++)
		{
			nodes[i].index = i + 1;
			nodes[i].value = hist[i];
		}
		nodes[HIST].index = -1;
	}

	char const* Predict()
	{
		int vote[6] = { 0 };
		int modelOrderAddOne[20] = { 1, 2, 1, 3, 1, 4, 1, 5, 2, 3, 2, 4, 2, 5, 3, 4, 3, 5, 4, 5 };
		//// get votes of each pose
		for (unsigned int i = 0; i < models.size(); i++)
		{
			double d = svm_predict(models[i], nodes);
			if (d > 0)
				vote[modelOrderAddOne[i * 2]]++;
			else
				vote[modelOrderAddOne[i * 2 + 1]]++;
		}
		//// Pick the max
		vote[0] = distance(vote, max_element(vote + 1, vote + 6));
		switch (vote[0]){		
		case 1:
			return "grab\n";
		case 2:
			return "throw\n";
		case 3:
			return "right rotation\n";
		case 4:
			return "left rotation\n";
		case 5:
			return "wave hand\n";
		default:
			return "ERROR!\n";
		}
	}

	int PredictNum()
	{
		int vote[6] = { 0 };
		int modelOrderAddOne[20] = { 1, 2, 1, 3, 1, 4, 1, 5, 2, 3, 2, 4, 2, 5, 3, 4, 3, 5, 4, 5 };
		//// get votes of each pose
		for (unsigned int i = 0; i < models.size(); i++)
		{
			double d = svm_predict(models[i], nodes);
			if (d > 0)
				vote[modelOrderAddOne[i * 2]]++;
			else
				vote[modelOrderAddOne[i * 2 + 1]]++;
		}
		//// Pick the max
		vote[0] = distance(vote, max_element(vote + 1, vote + 6));
		return vote[0];		
	}
};