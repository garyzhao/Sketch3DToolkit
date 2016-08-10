/*
 * sample_mesh_viewpoints_batch.cpp
 */

#include <iostream>
#include "retrieval.h"

using namespace std;
using namespace cv;
using namespace trimesh;
using namespace meshutil;

int main(int argc, char* argv[])
{	
	string target_mesh{ "C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Models\\m1054.off" };
	vector<string> target_sketch_vector({
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18563.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18567.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18573.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18576.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18578.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18581.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18585.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18586.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18587.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18592.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18593.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18595.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18597.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18598.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18599.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18602.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18605.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18611.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18613.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18614.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18616.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18618.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18627.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18628.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18629.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18633.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18634.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18637.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18638.png",
		"C:\\Users\\Gary\\Workspaces\\Sketch3DToolkit\\matlab\\dataset\\SHREC2013\\Sketches\\tree\\test\\18640.png",
	});

	ViewIndexer view_indexer;
	unique_ptr<BatchMeshWindow> main_window_ptr = initGLContext();

	cout << "[info]: Sampling mesh viewpoints." << endl;
	vector<ViewInfo> view_info_vector = sampleMeshViewpoints(target_mesh, *main_window_ptr);
	vector<pair<size_t, double>> target_view_idx_vector;
	target_view_idx_vector.reserve(target_sketch_vector.size());

	for (size_t i = 0; i < view_info_vector.size(); i++)
	{
		insert_image(view_info_vector[i].view_image, i, view_indexer);
		cout << "[info]: Inserting mseh view#" << i << " ..." << endl;
	}
	
	for (const auto& target_sketch : target_sketch_vector)
	{
		vector<pair<size_t, double>> result = retrieve_image(target_sketch, view_indexer);
		cout << "[info]: Retrieving image [" << target_sketch << "] ..." << endl;
		
		target_view_idx_vector.push_back(move(result[0]));
	}
	
	cout << "[info]: Ranking mesh views." << endl;
	vector<size_t> view_idx_vector = rankMeshViews(view_info_vector, target_view_idx_vector);
	for (int i = 0; i < view_idx_vector.size(); i++)
	{
		size_t idx = view_idx_vector[i];
		
		string viewName = to_string(i);
		imwrite(viewName + ".png", view_info_vector[idx].view_image);
		view_info_vector[idx].xf.write(viewName + ".xf");
	}

	clearGLContext(main_window_ptr);
	cout << "[info]: Done." << endl;

	getchar();
	return EXIT_SUCCESS;
}
