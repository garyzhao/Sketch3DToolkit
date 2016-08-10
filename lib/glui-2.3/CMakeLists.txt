include_directories(${GLUI_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
file(GLOB glui_srcs "src/*.cpp" "src/*.c")
add_library(glui32 ${glui_srcs})
target_link_libraries(glui32 freeglut)
