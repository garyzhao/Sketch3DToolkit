matlab_add_mex(
	NAME mexHashIndexer
	SRC mexHashIndexer.cpp mex_util.h
	LINK_TO imgproc hashing retrieval ${OpenCV_LIBS}
)

matlab_add_mex(
	NAME mexEvalRankLists
	SRC mexEvalRankLists.cpp mex_util.h
)

matlab_add_mex(
	NAME mexSelectMeshViews
	SRC mexSelectMeshViews.cpp mex_util.h
	LINK_TO imgproc hashing retrieval meshutil ${OpenCV_LIBS}
)

set_target_properties(mexHashIndexer PROPERTIES FOLDER "mex")
set_target_properties(mexEvalRankLists PROPERTIES FOLDER "mex")
set_target_properties(mexSelectMeshViews PROPERTIES FOLDER "mex")