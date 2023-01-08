# set minimum required version
cmake_minimum_required(VERSION 2.8.12)

# set project version
set(VERSION 1.0.0)

# set project name
project(HELLO)
project(HELLOLIB)

# print message
message(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
message(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})

# make subdirectory
add_subdirectory(src bin)
add_subdirectory(lib)

# set how to install the project
install(FILES COPYRIGHT README DESTINATION share/doc/)
install(PROGRAMS runhello.sh DESTINATION bin)
install(DIRECTORY doc/ DESTINATION share/doc/)
