include_directories(${GLUT_INCLUDE_DIR})
add_definitions(-DFREEGLUT_EXPORTS)

file(GLOB freeglut_srcs "src/*.c")
add_library(freeglut SHARED ${freeglut_srcs})