file(GLOB meshutil_hdrs "*.h")
file(GLOB meshutil_srcs "*.cpp" "*.cc")
add_library(meshutil ${meshutil_hdrs} ${meshutil_srcs})
target_link_libraries(meshutil imgproc trimesh2 glew glui32 freeglut)