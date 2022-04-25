aux_source_directory(./ SRC_LIST)

add_executable(ChatClient ${SRC_LIST})
target_link_libraries(ChatClient mysqlclient muduo_net muduo_base pthread)