add_subdirectory(glew-1.13.0)
add_subdirectory(freeglut-2.6.0)
add_subdirectory(glui-2.3)
add_subdirectory(trimesh2-2.12)

set_target_properties(glew PROPERTIES FOLDER "lib")
set_target_properties(freeglut PROPERTIES FOLDER "lib")
set_target_properties(glui32 PROPERTIES FOLDER "lib")
set_target_properties(trimesh2 PROPERTIES FOLDER "lib")