# set the source file to make the binary file.
set(SRC_LIST hello.c)

# set the binary file output path.
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
# set the binary file name. 
add_executable(hello ${SRC_LIST})

# set the include file search path, use alternative of them.
INCLUDE_DIRECTORIES(../lib/)
# if usd this method, you should run this first: export CMAKE_INCLUDE_PATH=../lib/ 
#find_path(myheader hello.h)
#if(myheader)
#    include_directories(${myheader})
#endif(myheader)

# set the library name to be linked
TARGET_LINK_LIBRARIES(hello hello_shared)

# tell how to install
install(TARGETS hello DESTINATION bin)
