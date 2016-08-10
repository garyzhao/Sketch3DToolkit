/*
 * mesh.cpp
 */

#include <map>
#include <tuple>

#include "retrieval.h"
#include "edge_util.h"

using namespace cv;
using namespace std;
using namespace trimesh;
using namespace meshutil;

Mat formatDepthImage(const Mat& image)
{
	assert(image.channels() == 1);
	assert(image.depth() == CV_32F);

	Mat image_bw(image, computeBoundingBox<float>(image));			// compute bbox
	image_bw = paddingImage(image_bw, 5);							// padding border
	resize(image_bw, image_bw, Size(IMAGE_SCALE, IMAGE_SCALE));		// resize image

	return image_bw;
}

double computeDepthIoU(const Mat& image1, const Mat& image2)
{
	assert(image1.rows == image2.rows);
	assert(image1.cols == image2.cols);

	Mat I = image1.mul(image2);
	Mat U = image1 + image2 - I;

	double u = sum(U)[0];
	if (u == 0)
		return 0;

	return sum(I)[0] / u;
}

Mat glDumpRenderbuffer()
{
	GLint V[4];
	glGetIntegerv(GL_VIEWPORT, V);
	GLint width = V[2], height = V[3];

	Mat img(height, width, CV_8UC3);

	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());
	glReadPixels(V[0], V[1], width, height, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	flip(img, img, 0);

	return img;
}

Mat glDumpDepthbuffer()
{
	GLint V[4];
	glGetIntegerv(GL_VIEWPORT, V);
	GLint width = V[2], height = V[3];

	Mat img(height, width, CV_32FC1);

	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());
	glReadPixels(V[0], V[1], width, height, GL_DEPTH_COMPONENT, GL_FLOAT, img.data);
	flip(img, img, 0);

	return img;
}

vec3d euler_angle(const xform& xf)
{
	vec3d vec;
	double x, y, z;

	if (xf(2, 0) != 1 && xf(2, 0) != -1)
	{
		y = -asin(xf(2, 0)); // y2 = M_PI - y;
		x = atan2(xf(2, 1) / cos(y), xf(2, 2) / cos(y));
		z = atan2(xf(1, 0) / cos(y), xf(0, 0) / cos(y));
	}
	else
	{
		z = 0;
		if (xf(2, 0) < 0)
		{
			y = M_PI / 2;
			x = z + atan2(xf(0, 1), xf(0, 2));
		}
		else
		{
			y = -M_PI / 2;
			x = -z + atan2(-xf(0, 1), -xf(0, 2));
		}
	}

	vec[0] = x;
	vec[1] = y;
	vec[2] = z;
	return vec;
}

double rot_angle(const trimesh::xform& xf)
{
	return acos((xf(0, 0) + xf(1, 1) + xf(2, 2) - 1) / 2.0);
}

vec3d rot_vector(const trimesh::xform& xf, double angle)
{
	vec3d vec;	
	vec[0] = (xf(2, 1) - xf(1, 2)) / (2.0 * sin(angle));
	vec[1] = (xf(0, 2) - xf(2, 0)) / (2.0 * sin(angle));
	vec[2] = (xf(1, 0) - xf(0, 1)) / (2.0 * sin(angle));

	return vec;
}

double central_angle_dist(const vec2d& v1, const vec2d& v2)
{
	double a, b, c, d, lat1, lat2, lon1, lon2, delta_lat, delta_lon;

	// lat1 = v1[0], lat2 = v2[0];
	// lon1 = v1[1], lon2 = v2[1];
	// delta_lon = abs(lon1 - lon2);
	// return (sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(delta_lon) + 1) / 2;

	lat1 = v1[0], lat2 = v2[0];
	lon1 = v1[1], lon2 = v2[1];
	delta_lat = abs(lat1 - lat2);
	delta_lon = abs(lon1 - lon2);

	a = sin(delta_lat / 2.0);
	b = sin(delta_lon / 2.0);
	c = cos(lat1) * cos(lat2) * b * b;
	d = 2 * asin(sqrt(a * a + c));

	return (cos(d) + 1) / 2.0;
}

//
// Main functions.
//
unique_ptr<BatchMeshWindow> initGLContext()
{
	// Initialize GLUT and create window
	unique_ptr<BatchMeshWindow> main_window = make_unique<BatchMeshWindow>(MESH_WIN_SIZE, MESH_WIN_SIZE);
	main_window->create("Sample Mesh Viewpoints", GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH, 0, nullptr);
	glutHideWindow();

	return main_window;
}

void clearGLContext(unique_ptr<BatchMeshWindow>& main_window_ptr)
{
	main_window_ptr->destroy();
	glutDestroyWindow(main_window_ptr->_window_id);
	glutExit();

	main_window_ptr.reset();
}

vector<ViewInfo> sampleMeshViewpoints(const std::string& mesh_path, BatchMeshWindow& main_window, int num)
{
	// Load mesh
	main_window._mesh_ptr = Mesh::read(mesh_path.c_str());
	if (main_window._mesh_ptr == nullptr)
	{
		cerr << "Failed to load mesh from " << mesh_path << "." << endl;
		abort();
	}

	// Initialize shape & sample viewpoints
	main_window._mesh_ptr->init_mesh();
	vector<ViewInfo> view_info_vector = main_window.sample_mesh_viewpoints(num);
	return move(view_info_vector);
}

vector<size_t> rankMeshViews(const vector<ViewInfo>& view_info_vector, vector<pair<size_t, double>>& view_idx_vector, int max_num, double th)
{
	// Remove duplicate
	map<size_t, double> view_idx_map;
	for (size_t i = 0; i < view_idx_vector.size(); i++)
	{
		size_t idx = view_idx_vector[i].first;
		double val = view_idx_vector[i].second;
		view_idx_map[idx] = max(view_idx_map[idx], val);
	}

	view_idx_vector.clear();
	view_idx_vector.reserve(view_idx_map.size());
	for (const auto& kv : view_idx_map)
		view_idx_vector.push_back(kv);
	sort(view_idx_vector.begin(), view_idx_vector.end(), [](const pair<size_t, double>& a, const pair<size_t, double>& b) { return b.second < a.second; });

	// Select top 50% max
	vector<tuple<size_t, double, double>> view_score_vector;
	view_score_vector.reserve(view_idx_vector.size());
	double max_val = view_idx_vector.front().second;

	for (size_t i = 0; i < view_idx_vector.size(); i++)
	{
		if (view_idx_vector[i].second < max_val * 0.5 && i > max_num)
			break;

		view_score_vector.emplace_back(view_idx_vector[i].first, view_idx_vector[i].second, 0.0);
	}
	
	// Start ranking
	vector<size_t> idx_vector;
	while (idx_vector.size() < max_num)
	{
		size_t idx = get<0>(view_score_vector.front());
		idx_vector.push_back(idx);
		view_score_vector.erase(view_score_vector.begin());

		for (auto& view_val : view_score_vector)
		{
			size_t idx_ = get<0>(view_val);

			double IoU = computeDepthIoU(view_info_vector[idx].depth_image, view_info_vector[idx_].depth_image);
			double dist = central_angle_dist(view_info_vector[idx].position, view_info_vector[idx_].position);
			get<2>(view_val) = max(get<2>(view_val), sqrt(IoU) * dist);
		}

		sort(view_score_vector.begin(), view_score_vector.end(), [](const tuple<size_t, double, double>& a, const tuple<size_t, double, double>& b) {
			// return get<1>(b) * (1 - get<2>(b)) < get<1>(a) * (1 - get<2>(a));
			// return 1 - get<2>(b) < 1 - get<2>(a);
			auto f = [](double x) -> double { return exp(-x * x / (2 * 0.3 * 0.3)); };
			return f(get<2>(b)) < f(get<2>(a));
		});

		if (view_score_vector.size() == 0) break;
		if (get<2>(view_score_vector[0]) > th) break;
	}

	return move(idx_vector);
}
