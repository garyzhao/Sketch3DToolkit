/*
 * mexEvalRankLists.cpp
 */

#include <vector>
#include <fstream>
#include <algorithm>

#include "mex_util.h"

using namespace std;

//
// Helper functions.
//
vector<string> readRankList(const string& filePath)
{
	ifstream fin(filePath.c_str());
	if (!fin) {
		string errMsg = "Error: failed to open file " + filePath;
		mexErrMsgTxt(errMsg.c_str());
	}

	double dist;
	string fileName;
	vector<string> fileList;

	fin >> fileName;
	while (fin >> fileName >> dist)
		fileList.push_back(fileName);
	fin.close();

	return fileList;
}

template<typename T>
vector<T> cumsum(const vector<T>& vec)
{
	T pre = 0;
	vector<T> out(vec.size());
	for (size_t i = 0; i < vec.size(); i++)
	{
		out[i] = vec[i] + pre;
		pre = out[i];
	}
	return out;
}

//
// Main function.
//
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs < 4) 
	{
		mexErrMsgTxt("Expected rankListsPath, queryClass, targetClass and C for evaluation.");
		return;
	}

	string rankListsPath = to_string(prhs[0]);
	const mxArray* queryClass = prhs[1];
	const mxArray* targetClass = prhs[2];
	const double* C = (const double*)mxGetData(prhs[3]);

	size_t queryClassNum = mxGetM(queryClass);
	size_t targetClassNum = mxGetM(targetClass);
	double C_MAX = *max_element(C, C + queryClassNum);

	// create output matrix
	mxArray* P_PNT	= mxCreateNumericMatrix(queryClassNum, size_t(C_MAX), mxDOUBLE_CLASS, mxREAL);
	mxArray* AP		= mxCreateNumericMatrix(1, queryClassNum, mxDOUBLE_CLASS, mxREAL);
	mxArray* NN		= mxCreateNumericMatrix(1, queryClassNum, mxDOUBLE_CLASS, mxREAL);
	mxArray* FT		= mxCreateNumericMatrix(1, queryClassNum, mxDOUBLE_CLASS, mxREAL);
	mxArray* ST		= mxCreateNumericMatrix(1, queryClassNum, mxDOUBLE_CLASS, mxREAL);
	mxArray* DCG	= mxCreateNumericMatrix(1, queryClassNum, mxDOUBLE_CLASS, mxREAL);
	mxArray* E		= mxCreateNumericMatrix(1, queryClassNum, mxDOUBLE_CLASS, mxREAL);

	double* pP_PNT	= (double*)mxGetData(P_PNT);
	double* pAP		= (double*)mxGetData(AP);
	double* pNN		= (double*)mxGetData(NN);
	double* pFT		= (double*)mxGetData(FT);
	double* pST		= (double*)mxGetData(ST);
	double* pDCG	= (double*)mxGetData(DCG);
	double* pE		= (double*)mxGetData(E);

	for (size_t i = 0; i < queryClassNum; i++)
	{
		string queryName = to_string(mxGetCell(queryClass, i));
		string queryClassName = to_string(mxGetCell(queryClass, i + queryClassNum));
		
		vector<string> fileList = readRankList(rankListsPath + "/" + queryName);
		vector<double> vG(fileList.size(), 0);
		for (size_t j = 0; j < fileList.size(); j++)
		{
			for (size_t k = 0; k < targetClassNum; k++)
			{
				string targetName = to_string(mxGetCell(targetClass, k));
				if (fileList[j] == targetName)
				{
					string targetClassName = to_string(mxGetCell(targetClass, k + targetClassNum));
					if (queryClassName == targetClassName)
						vG[j] = 1;
					break;
				}
			}
		}

		double P_PNT_SUM = 0;
		vector<double> vG_SUM = cumsum(vG);

		for (size_t j = 0; j < C[i]; j++)
		{
			size_t R_PNT;
			for (size_t k = 0; k < vG_SUM.size(); k++)
				if (vG_SUM[k] == j + 1)
				{
					R_PNT = k;
					break;
				}

			double PNT = vG_SUM[R_PNT] / double(R_PNT + 1);
			pP_PNT[i + j * queryClassNum] = PNT;
			P_PNT_SUM += PNT;
		}

		pAP[i] = P_PNT_SUM / C[i];
		pNN[i] = vG[0];
		pFT[i] = vG_SUM[size_t(C[i] - 1)] / C[i];
		pST[i] = vG_SUM[size_t(2 * C[i] - 1)] / C[i];

		double P_32 = vG_SUM[31] / 32;
		double R_32 = vG_SUM[31] / C[i];
		if (P_32 == 0 && R_32 == 0)
			pE[i] = 0;
		else
			pE[i] = 2 * P_32 * R_32 / (P_32 + R_32);

		double normVal = 1;
		double DCG_I = vG[0];
		for (double j = 1; j < C[i]; j++)
			normVal += 1 / log2(j + 1);
		for (size_t j = 1; j < vG.size(); j++)
			DCG_I += 1 / log2(double(j + 1)) * vG[j];

		pDCG[i] = DCG_I / normVal;
	}

	plhs[0] = P_PNT;
	plhs[1] = AP;
	plhs[2] = NN;
	plhs[3] = FT;
	plhs[4] = ST;
	plhs[5] = DCG;
	plhs[6] = E;
}