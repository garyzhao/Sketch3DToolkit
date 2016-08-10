/*
 * mexHashIndexer.cpp
 */

#include <memory>
#include "mex_util.h"
#include "retrieval.h"

using namespace std;

//
// Helper functions.
//
void export_result(const string& filePath, const string& imageName, const vector<pair<string, double>>& result)
{
	ofstream fout(filePath.c_str(), ios::out | ios::trunc);

	if (!fout)
	{
		cerr << "Error: Failed to open file. [" << filePath << "]" << endl;
		abort();
	}

	fout << imageName << endl;
	fout << fixed << setprecision(8);
	for (const auto& kv : result)
		fout << " " << kv.first << "\t" << 1.0 / kv.second << endl;

	fout.close();
}

//
// Main functions(commads).
//
static unique_ptr<Indexer> indexerPtr(nullptr);

void init(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	indexerPtr = make_unique<Indexer>();
}

void load(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs != 1) {
		mexErrMsgTxt("Expected filePath to load indexer data.");
		return;
	}

	indexerPtr = make_unique<Indexer>();
	string filePath = ::to_string(prhs[0]);
	indexerPtr->read(filePath);
	mexPrintf("Loaded indexer data from %s.\n", filePath.c_str());
}

void insert(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs != 2) {
		mexErrMsgTxt("Expected imagePath and imageName for indexing.");
		return;
	}
	if (indexerPtr == nullptr) {
		mexErrMsgTxt("Initialize indexer object first.");
		return;
	}

	string imagePath = ::to_string(prhs[0]);
	string imageName = ::to_string(prhs[1]);
	insert_image(imagePath, imageName, *indexerPtr);
	mexPrintf("Inserted image data: %s.\n", imageName.c_str());
}

void save(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs != 1) {
		mexErrMsgTxt("Expected filePath to save indexer data.");
		return;
	}
	if (indexerPtr == nullptr) {
		mexErrMsgTxt("Initialize indexer object first.");
		return;
	}

	string filePath = ::to_string(prhs[0]);
	indexerPtr->write(filePath);
	mexPrintf("Saved indexer data to %s.\n", filePath.c_str());
}

void retrieve_sketch(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs < 2) {
		mexErrMsgTxt("Expected imagePath and imageName for retrieving.");
		return;
	}
	if (indexerPtr == nullptr) {
		mexErrMsgTxt("Load indexer object first.");
		return;
	}

	string imagePath = ::to_string(prhs[0]);
	string imageName = ::to_string(prhs[1]);
	vector<pair<string, double>> dataVector = retrieve_image(imagePath, *indexerPtr);
	
	if (nlhs > 0)
	{
		size_t dataVectorNum = dataVector.size();
		mxArray* dataCellPtr = mxCreateCellMatrix(dataVector.size(), 2);
		mxArray* cellValPtr = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
		double* cellVal = (double*)mxGetData(cellValPtr);

		for (size_t i = 0; i < dataVectorNum; i++)
		{
			*cellVal = dataVector[i].second;
			mxSetCell(dataCellPtr, i, mxCreateString(dataVector[i].first.c_str()));
			mxSetCell(dataCellPtr, i + dataVectorNum, mxDuplicateArray(cellValPtr));
		}
		plhs[0] = dataCellPtr;
	}
	
	// Export result to data file
	if (nrhs > 2)
	{
		string filePath = ::to_string(prhs[2]);
		export_result(filePath, imageName, dataVector);
	}

	mexPrintf("Retrieved sketch data: %s.\n", imageName.c_str());
}

void retrieve_model(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs < 2) {
		mexErrMsgTxt("Expected meshPath and meshName for retrieving.");
		return;
	}
	if (indexerPtr == nullptr) {
		mexErrMsgTxt("Load indexer object first.");
		return;
	}

	string imagePath = ::to_string(prhs[0]);
	string imageName = ::to_string(prhs[1]);
	vector<pair<string, double>> dataVector = retrieve_mesh(imagePath, *indexerPtr, [](string imageName) -> string {
		size_t idx = imageName.find_first_of("_");
		assert(idx != string::npos);
		return imageName.substr(0, idx);
	});

	if (nlhs > 0)
	{
		size_t dataVectorNum = dataVector.size();
		mxArray* dataCellPtr = mxCreateCellMatrix(dataVector.size(), 2);
		mxArray* cellValPtr = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
		double* cellVal = (double*)mxGetData(cellValPtr);

		for (size_t i = 0; i < dataVectorNum; i++)
		{
			*cellVal = dataVector[i].second;
			mxSetCell(dataCellPtr, i, mxCreateString(dataVector[i].first.c_str()));
			mxSetCell(dataCellPtr, i + dataVectorNum, mxDuplicateArray(cellValPtr));
		}
		plhs[0] = dataCellPtr;
	}

	// Export result to data file
	if (nrhs > 2)
	{
		string filePath = ::to_string(prhs[2]);
		export_result(filePath, imageName, dataVector);
	}

	mexPrintf("Retrieved model data: %s.\n", imageName.c_str());
}

//
// Define commads for mexFunction.
//
DECLARE_MEX_COMMAND_BEGIN
DECLARE_MEX_COMMAND(init)
DECLARE_MEX_COMMAND(save)
DECLARE_MEX_COMMAND(load)
DECLARE_MEX_COMMAND(insert)
DECLARE_MEX_COMMAND(retrieve_sketch)
DECLARE_MEX_COMMAND(retrieve_model)
DECLARE_MEX_COMMAND_END
DECLARE_MEX_FUNCTION