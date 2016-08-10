cmake_minimum_required(VERSION 3.5)
project(Sketch3DToolkit)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(MSVC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/msvc)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/module)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(GLUT_INCLUDE_DIR ${LIB_DIR}/freeglut-2.6.0/include)
set(GLEW_INCLUDE_DIR ${LIB_DIR}/glew-1.13.0/include)
set(GLUI_INCLUDE_DIR ${LIB_DIR}/glui-2.3/src/include)
set(TRIMESH2_INCLUDE_DIR ${LIB_DIR}/trimesh2-2.12/include)
set(LIB_INCLUDE_DIR ${GLUT_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${GLUI_INCLUDE_DIR} ${TRIMESH2_INCLUDE_DIR})

include_directories(${TRIMESH2_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${GLUI_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
include_directories(
	${MODULE_DIR}/imgproc
	${MODULE_DIR}/meshutil
	${MODULE_DIR}/hashing
	${MODULE_DIR}/retrieval
	${SRC_DIR}
)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
	
	set(MSVC_USER_CONFIG ${MSVC_DIR}/project.vcxproj.user.in)
endif(MSVC)

if(WIN32)
	set(OpenCV_DIR ${MSVC_DIR}/opencv-2.4.12)
	set(OpenCV_STATIC OFF)
endif(WIN32)

find_package(OpenCV REQUIRED)
find_package(Matlab COMPONENTS MX_LIBRARY)
if(Matlab_FOUND)
	add_subdirectory(mex)
endif(Matlab_FOUND)

#find_package(OpenMP)
#if(OPENMP_FOUND)
#	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#endif(OPENMP_FOUND)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(module)
