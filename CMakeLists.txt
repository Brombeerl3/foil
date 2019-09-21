cmake_minimum_required(VERSION 3.12)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
# enter project name here - defaults to folder name
#project(${ProjectId}) # uncomment me for c++ project
project(${ProjectId} C) # uncomment me for c project

# add sources you want to compile in your project here
set(SOURCE global.c main.c input.c output.c state.c literal.c evaluatelit.c search.c determinate.c order.c join.c utility.c finddef.c interpret.c prune.c constants.c defns.i extern.i)
#set(CMAKE_CXX_STANDARD 17) # for cpp standard 17
#set(CMAKE_CXX_STANDARD 14) # for cpp standard 14
set(CMAKE_C_STANDARD 99) # for c standard 99

# for executable, uncomment next line
add_executable(${PROJECT_NAME} ${SOURCE})

# for static library, uncomment next line
#add_library(${PROJECT_NAME} ${SOURCE})

# for shared library, uncomment next line
#add_library(${PROJECT_NAME} SHARED ${SOURCE})

# link external libraries to project output (e.g. math library libm)
target_link_libraries(${PROJECT_NAME} m)
