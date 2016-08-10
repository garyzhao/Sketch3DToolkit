/*
 * Implementation of the Model class, a class representing a loaded mesh and its view-independent and view-dependent data.
 */

#include "Model.h"
#include "mesh_info.h"
#include "vertex_info.h"

using namespace std;
using namespace trimesh;
using namespace meshutil;

/**
 * Constructor: construct a model
 * @param filename : the filesystem location of the file containing mesh_ data
 */
Model::Model(const char* filename)
{
	// read mesh_ from file
	TriMesh* mesh = TriMesh::read(filename);
	// TriMesh2 library can calculate some vertex-info for us
	mesh_ = shared_ptr<TriMesh>(mesh);
	init();
}

Model::Model(shared_ptr<TriMesh> mesh)
{
	mesh_ = mesh;
	init();
}

void Model::init()
{
	if (mesh_ != nullptr)
	{
		mesh_->need_across_edge();
		mesh_->need_normals();
		mesh_->need_tstrips();
		mesh_->need_bsphere();
		mesh_->need_curvatures();
		mesh_->need_across_edge();
		mesh_->need_dcurv();
		clearDrawers();
		clearViewDependentData();
		computeViewIndependentData();
		setupVBOs();
	}
}

void Model::clear()
{
	clearDrawers();
	clearViewDependentData();
	mesh_.reset();
}

/**
 * Destructor: destruct a model
 */
Model::~Model()
{
	clearVBOs();
}

/**
 * Draw a given model using it's current draw stack.
 *
 * @param camera_position: the current position of the camera, in 3d coordinates
 */
void Model::draw(vec camera_position) {
	// clear all view-dependent buffers: drawers_ will fill them as necessary
	clearViewDependentData();
	// for every drawer in the draw stack, call draw function
	for (unsigned int i = 0; i < drawers_.size(); i++) {
		drawers_[i]->draw(this, camera_position);
	}
}

/**
 * Push back a drawer into this model's drawing stack
 * @param: d : the drawer you want to push
 */
void Model::pushDrawer(Drawer* d) {
	drawers_.push_back(d);
}

/**
 * Pop a drawer from this model's drawing stack
 */
void Model::popDrawer() {
	drawers_.pop_back();
}

/**
 * Clear all drawers from the drawer stack
 */
void Model::clearDrawers() {
	drawers_.clear();
}

/**
 * Compute NdotV for all vertices, given a camera position
 */
void Model::needNdotV(vec camera_position)
{
	if (ndotv_.empty()) {
		compute_ndotv(*mesh_, camera_position, ndotv_);
	}
}

/**
 * Compute radial curvature and the numerator/denominator of the directional curvature derivative
 * given a camera standpoint
 *
 * @param: camera_position : the camera standpoint
 * @param: sc_threshold : a filtering threshold for small curvatures
 */
void Model::needCurvDerivatives(vec camera_position, float sc_threshold)
{
	if (kr_.empty() || num_.empty() || den_.empty()){
		compute_CurvDerivatives(*mesh_, camera_position, kr_, num_, den_, sc_threshold);
	}
}

/**
 * Compute the view independent data for this model.
 */
void Model::computeViewIndependentData(){
	// reserve memory chunks for per-vertex info
	int n = mesh_->vertices.size();
	ndotv_.resize(n);
	kr_.resize(n);
	num_.resize(n);
	den_.resize(n);
	// The other view-indipendent data, we have to compute ourselves
	std::cout << "Computing face normals... ";
	computeFaceNormals(*mesh_, facenormals_);
	std::cout << "Done" << std::endl << "Computing feature size... ";
	feature_size_ = computeFeatureSize(*mesh_);
	std::cout << "Done" << std::endl;
}

/**
 * Clear the buffers containing the view dependent data
 */
void Model::clearViewDependentData() {
	ndotv_.clear();
	kr_.clear();
	num_.clear();
	den_.clear();
}

/**
 * Transfer vertex/normal info into GPU memory as STATIC_DRAW data in Vertex Buffer Objects (VBO's).
 */
void Model::setupVBOs() {
	int bufferSize;
	// load vertex positions into VBO buffer
	glGenBuffersARB(1, &vbo_positions_);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_positions_);
	// static draw data, we're not going to change vertex information
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh_->vertices.size()*sizeof(float) * 3, &(mesh_->vertices[0]), GL_STATIC_DRAW_ARB);
	glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &bufferSize);
	std::cout << "Vertex array loaded in VBO: " << bufferSize << " bytes, #" << vbo_positions_ << std::endl;
	// do the same for normals
	glGenBuffersARB(1, &vbo_normals_);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_normals_);
	// static draw data, we're not going to change vertex information
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh_->normals.size()*sizeof(float) * 3, &(mesh_->normals[0]), GL_STATIC_DRAW_ARB);
	glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &bufferSize);
	std::cout << "Normal array loaded in VBO: " << bufferSize << " bytes, #" << vbo_normals_ << std::endl;
	// unbind buffers to prevent fudging up pointer arithmetic
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

/**
 * Clear STATIC_DRAW data in Vertex Buffer Objects (VBO's).
 */
void Model::clearVBOs()
{
	glDeleteBuffersARB(1, &vbo_positions_);
	glDeleteBuffersARB(1, &vbo_normals_);
	std::cout << "Deleted VBO data" << std::endl;
}