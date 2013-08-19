cmake_minimum_required (VERSION 2.8)
project (libGEXF)

file(GLOB_RECURSE SOURCE_gexf "./libgexf/*.cpp")
include_directories("/usr/include/libxml2")

add_library(gexf SHARED ${SOURCE_gexf})
