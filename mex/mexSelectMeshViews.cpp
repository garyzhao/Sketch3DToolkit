/*
 * mexSelectMeshViews.cpp
 */

#include "mex_util.h"
#include "retrieval.h"

using namespace std;

static unique_ptr<BatchMeshWindow> mainWindowPtr(nullptr);

//
// Main function.
//
void init_context(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	mainWindowPtr = initGLContext();
}

void clear_context(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	clearGLContext(mainWindowPtr);
}

void select_view(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs < 4)
	{
		mexErrMsgTxt("Expected modelPath, modelName, sketchesPath and outputPath for selection.");
		return;
	}

	string target_mesh_path = to_string(prhs[0]);
	string target_mesh_name = to_string(prhs[1]);
	string output_path = to_string(prhs[3]);
	
	size_t target_sketch_num = mxGetM(prhs[2]);
	vector<string> target_sketch_vector;
	target_sketch_vector.reserve(target_sketch_num);
	for (size_t i = 0; i < target_sketch_num; i++)
		target_sketch_vector.push_back(to_string(mxGetCell(prhs[2], i)));

	mexPrintf("Sampling Mesh Views: %s.\n", target_mesh_name.c_str());

	ViewIndexer view_indexer;
	vector<ViewInfo> view_info_vector = sampleMeshViewpoints(target_mesh_path, *mainWindowPtr);
	vector<pair<size_t, double>> target_view_idx_vector;
	target_view_idx_vector.reserve(target_sketch_vector.size());

	for (size_t i = 0; i < view_info_vector.size(); i++)
		insert_image(view_info_vector[i].view_image, i, view_indexer);

	for (const auto& target_sketch : target_sketch_vector)
	{
		vector<pair<size_t, double>> result = retrieve_image(target_sketch, view_indexer);
		target_view_idx_vector.push_back(move(result[0]));
	}

	mexPrintf("Ranking Mesh Views: %s.\n", target_mesh_name.c_str());
	vector<size_t> view_idx_vector = rankMeshViews(view_info_vector, target_view_idx_vector);
	for (int i = 0; i < view_idx_vector.size(); i++)
	{
		size_t idx = view_idx_vector[i];

		string view_name = output_path + "/" + target_mesh_name + "_" + to_string(i);
		imwrite(view_name + ".png", view_info_vector[idx].view_image);
		view_info_vector[idx].xf.write(view_name + ".xf");
	}
}

//
// Define commads for mexFunction.
//
DECLARE_MEX_COMMAND_BEGIN
DECLARE_MEX_COMMAND(init_context)
DECLARE_MEX_COMMAND(clear_context)
DECLARE_MEX_COMMAND(select_view)
DECLARE_MEX_COMMAND_END
DECLARE_MEX_FUNCTION