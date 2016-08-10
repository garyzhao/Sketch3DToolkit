add_subdirectory(meshutil)
add_subdirectory(imgproc)
add_subdirectory(hashing)
add_subdirectory(retrieval)

set_target_properties(meshutil PROPERTIES FOLDER "module")
set_target_properties(imgproc PROPERTIES FOLDER "module")
set_target_properties(hashing PROPERTIES FOLDER "module")
set_target_properties(retrieval PROPERTIES FOLDER "module")