cmake_minimum_required(VERSION 3.0)  
project(testmuduo)


# 配置编译选项
#  set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# 配置头文件搜索路径
#include_directories(/usr/local/include)

# 配置库文件搜索路径
#link_directories(/usr/local/lib)


set(SRC_LIST muduo_server.cpp)

# 把.指定路径下下的所有源文件名字 放入变量SRC_LIST中
# aux_source_directory(. SRC_LIST)


# 设置可执行文件执行目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


# 生成可执行文件
add_executable(server ${SRC_LIST})

# 表示server 需要的库文件
target_link_libraries(server muduo_net muduo_base pthread)