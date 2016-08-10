file(GLOB hashing_hdrs "*.h")
file(GLOB hashing_srcs "*.cpp")
add_library(hashing ${hashing_hdrs} ${hashing_srcs})