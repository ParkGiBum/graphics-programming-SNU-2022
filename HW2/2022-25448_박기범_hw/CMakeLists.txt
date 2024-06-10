cmake_minimum_required(VERSION 3.10)
cmake_policy(VERSION 3.10)
project("HW2" C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(PROJECT_ORIGIN "../../")

file(GLOB SOURCE
    "src/*.h"
    "src/*.cpp"
)

set(SOURCE ${SOURCE} ../glad.c)
set(NAME "main")

link_directories(${PROJECT_ORIGIN}/lib)

if(WIN32)
  set(LIBS glfw3 opengl32 freetype)
endif(WIN32)

if(UNIX)
  find_package(PkgConfig REQUIRED)
  pkg_search_module(GLFW REQUIRED glfw3)
  find_package(glm REQUIRED)
  set(LIBS ${GLFW_STATIC_LIBRARIES} freetype)
  include_directories(${GLFW_INCLUDE_DIRS})
  include_directories(${GLM_INCLUDE_DIRS})
endif(UNIX)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")
add_executable(${NAME} ${SOURCE})
target_link_libraries(${NAME} ${LIBS})
include_directories(${PROJECT_ORIGIN}/includes)