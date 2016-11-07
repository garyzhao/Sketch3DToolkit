macro(add_mex name)	
	matlab_add_mex(
		NAME ${name}
		SRC ${ARGN} mex_util.h
		LINK_TO ${OpenCV_LIBS} meshutil imgproc hashing retrieval
	)
	set_target_properties(${name} PROPERTIES FOLDER "mex")
endmacro()

add_mex(mexHashIndexer mexHashIndexer.cpp)
add_mex(mexEvalRankLists mexEvalRankLists.cpp)
add_mex(mexSelectMeshViews mexSelectMeshViews.cpp)