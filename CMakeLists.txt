cmake_minimum_required(VERSION 3.21)
project(tinySTL)

set(CMAKE_CXX_STANDARD 14)

# build type
# set(CMAKE_BUILD_TYPE release)

# aux_source_directory(./test DIR_FILE)

add_executable(tinySTL main.cpp)

# 添加头文件所在路径，这时候cpp中就可以直接引用，而不用管路径了
target_include_directories(tinySTL PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(tinySTL PUBLIC ${PROJECT_SOURCE_DIR}/src/base)
target_include_directories(tinySTL PUBLIC ${PROJECT_SOURCE_DIR}/test)