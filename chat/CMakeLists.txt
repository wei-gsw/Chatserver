cmake_minimum_required(VERSION 3.0)
project(chat)

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 配置最终的可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/mysql)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
link_directories(/usr/lib64/mysql)

# 加载子目录
add_subdirectory(src)


add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)



#库文件的搜索路径
#link_directories()

#设置需要编译的 源文件列表 
#set(SRC_LIST ./muduo_server.cpp)

#也可以吧.指定路径下所有源文件 名字放入SRC_LIST里面
#aux_source_directory(. SRC_LIST)

#生成可执行文件server，由SRC_LIST变量所定义的源文件 -o 编译而来
#add_executable(server ${SRC_LIST})
#server 连接库
#target_link_libraries(server muduo_net muduo_base pthread)

