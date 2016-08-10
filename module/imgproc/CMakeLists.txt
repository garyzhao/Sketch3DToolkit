file(GLOB imgproc_hdrs "*.h")
file(GLOB imgproc_srcs "*.cc" "*.cpp")
add_library(imgproc ${imgproc_hdrs} ${imgproc_srcs})
target_link_libraries(imgproc ${OpenCV_LIBS})