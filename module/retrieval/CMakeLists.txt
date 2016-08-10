file(GLOB retrieval_hdrs "*.h")
file(GLOB retrieval_srcs "*.cpp")
add_library(retrieval ${retrieval_hdrs} ${retrieval_srcs})
target_link_libraries(retrieval hashing meshutil)