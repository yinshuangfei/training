# set source file
set(LIBHELLO_SRC hello.c)
# set library output path
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# set the library name
add_library(hello_shared SHARED ${LIBHELLO_SRC})
add_library(hello_static STATIC ${LIBHELLO_SRC})

# set the shared library version
SET_TARGET_PROPERTIES(hello_shared PROPERTIES VERSION 2.4 SOVERSION 1)

# rename the shared and static library name.
SET_TARGET_PROPERTIES(hello_shared PROPERTIES OUTPUT_NAME "hello")
SET_TARGET_PROPERTIES(hello_static PROPERTIES OUTPUT_NAME "hello")

#GET_TARGET_PROPERTY(OUTPUT_VALUE hello_static OUTPUT_NAME)
#MESSAGE(STATUS  "This is the hello_static OUTPUT_NAME:" ${OUTPUT_VALUE})

# set how to install
install(TARGETS hello_shared hello_static
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(FILES hello.h DESTINATION include/hello)
