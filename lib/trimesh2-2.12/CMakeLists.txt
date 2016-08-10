include_directories(${TRIMESH2_INCLUDE_DIR})
file(GLOB trimesh2_srcs "libsrc/*.cc")
add_library(trimesh2 ${trimesh2_srcs})