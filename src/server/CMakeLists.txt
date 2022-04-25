# 设置可执行文件生成源文件路径,定义了一个SRC_LIST变量，包含了该目录下所有源文件的路径
aux_source_directory(./db SRC_LIST)
aux_source_directory(./model SRC_LIST)
aux_source_directory(./ SRC_LIST)

# 生成可执行文件
add_executable(ChatServer ${SRC_LIST})
# 指定可执行文件链接时所需要的库文件
target_link_libraries(ChatServer mysqlclient muduo_net muduo_base pthread)
