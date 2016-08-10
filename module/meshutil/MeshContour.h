/*
 * MeshContour.h
 */

#ifndef _MESH_CONTOUR_H_
#define _MESH_CONTOUR_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <memory>

namespace meshutil {

	class MeshContour {

	public:

		struct Node { int i; float x, y, o, m; };

		MeshContour() {}

		static std::shared_ptr<MeshContour> dump_glbuffer();
		static std::shared_ptr<MeshContour> load_image(cv::Mat& img);

		cv::Mat& data() { return _E_data; }
		cv::Mat visualize_segIdx() const;
		cv::Mat visualize_keySegIdx(const std::vector<int>& key_idx) const;

		void cluster();
		std::vector<std::vector<int>> collect_harris_nodes_walks(int l);
		std::vector<std::vector<int>> collect_all_nodes_walks(int l);

	private:

		cv::Mat _E_data;
		cv::Mat _O_data;

		int _segNum = 0;
		cv::Mat _segIdx;
		std::vector<Node> _segNodes;
		std::vector<std::vector<int>> _segAffIdx;

		void collect_node_walks(std::vector<std::vector<int>>& walks, const std::vector<int>& w, int l);
		int find_closet_segIdx(int x, int y);
	};

};

#endif // _MESH_CONTOUR_H_