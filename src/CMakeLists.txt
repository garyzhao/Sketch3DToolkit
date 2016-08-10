include_directories(${LIB_INCLUDE_DIR})

add_executable(retrieve_sketch_example retrieve_sketch_example.cpp)
target_link_libraries(retrieve_sketch_example ${OpenCV_LIBS} imgproc hashing retrieval)
configure_file(${MSVC_USER_CONFIG} ${CMAKE_CURRENT_BINARY_DIR}/retrieve_sketch_example.vcxproj.user @ONLY)

add_executable(view_mesh_example view_mesh_example.cpp)
target_link_libraries(view_mesh_example meshutil glew freeglut retrieval)
configure_file(${MSVC_USER_CONFIG} ${CMAKE_CURRENT_BINARY_DIR}/view_mesh_example.vcxproj.user @ONLY)

add_executable(sample_mesh_viewpoints_batch sample_mesh_viewpoints_batch.cpp)
target_link_libraries(sample_mesh_viewpoints_batch ${OpenCV_LIBS} meshutil glew freeglut imgproc hashing retrieval)
configure_file(${MSVC_USER_CONFIG} ${CMAKE_CURRENT_BINARY_DIR}/sample_mesh_viewpoints_batch.vcxproj.user @ONLY)

set_target_properties(retrieve_sketch_example PROPERTIES FOLDER "src")
set_target_properties(view_mesh_example PROPERTIES FOLDER "src")
set_target_properties(sample_mesh_viewpoints_batch PROPERTIES FOLDER "src")
