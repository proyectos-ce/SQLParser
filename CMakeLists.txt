cmake_minimum_required(VERSION 3.7)
project(parser)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp cmdline.h WhereSelector.h UpdateDescriptor.h ColumnDescriptor.h SelectDescriptor.h ValuePair.h Where.h ConnectionManager.cpp ConnectionManager.h)
add_executable(parser ${SOURCE_FILES})

find_package(Boost 1.40 COMPONENTS regex REQUIRED)
include_directories( ${Boost_INCLUDE_DIRS} )


target_link_libraries(parser ${Boost_LIBRARIES} )
