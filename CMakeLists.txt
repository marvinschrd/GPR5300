cmake_minimum_required (VERSION 2.16)
project(GPR5300)


set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS On)

find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp include/*.h)
add_library(CommonLib STATIC ${SOURCES})
target_include_directories(CommonLib PUBLIC "include/")
target_link_libraries(CommonLib PUBLIC SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(CommonLib PUBLIC imgui::imgui)
target_link_libraries(CommonLib PUBLIC glad::glad)
target_link_libraries(CommonLib PUBLIC ${OPENGL_LIBRARIES})

file(GLOB_RECURSE main_files main/*.cpp)
foreach(test_file ${main_files})
	get_filename_component(test_name ${test_file} NAME_WE)
	add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE CommonLib)
endforeach()