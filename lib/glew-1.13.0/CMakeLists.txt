include_directories(${GLEW_INCLUDE_DIR})
add_definitions(-DGLEW_BUILD)
add_library(glew SHARED src/glew.c include/GL/glew.h include/GL/wglew.h)
target_link_libraries(glew opengl32)