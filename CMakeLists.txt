cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# "../" --> output directory relative same to running in IDE
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)

if (MSVC)
  set(CMAKE_CXX_STANDARD 11)
endif()

project(zweidee)
message("SRC_DIR -> " ${CMAKE_CURRENT_SOURCE_DIR})
message("PRJ_NAME -> " ${CMAKE_PROJECT_NAME})
#message("PRJ_NAME -> " ${PROJECT_NAME})


set(glew "glew-2.1.0")
set(CMAKE_PREFIX_PATH "./lib/${glew}")
set(CMAKE_LIBRARY_PATH "./lib/${glew}/lib/release/x64/")
find_package(GLEW REQUIRED)

#message("GLEW -> " ${GLEW::GLEW})
message("GLEW -> " ${GLEW_LIBRARIES})

include_directories(.
  ./src
  ${GLEW_INCLUDE_DIRS}
  ./lib/glm/glm
  ./src/common
  ./src/engine
  #./include/imgui
  #./include/imgui/src
  #./include/imgui/debuggers
  #./src/features
  #./src/math
  #./src/pointcloud
  #./src/util
  #./src/frs
)

link_directories(
  # glew
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/${glew}/lib/release/x64
  # glm is source build
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

#link_libraries(${GLEW_LIBRARIES})

# add WIN32 to remove console window ->
add_executable (${CMAKE_PROJECT_NAME} WIN32
  # wrapper
  ./src/main.cpp
  # app
  ./src/glenz.cpp
  ./src/glenz.h
  # uses ->
  ./src/common/lodepng.h
  ./src/engine/engine.cpp
  ./src/engine/engine.h
  ./src/engine/game.h
  ./src/engine/zweidee.h
  ./src/engine/zweidee_draw.h
  ./src/win/Resource.h
  ./src/win/glenz.rc
)

target_link_libraries(${CMAKE_PROJECT_NAME}
  opengl32.lib glu32.lib #${GLEW_LIBRARIES} GLEW::GLEW #glew32.lib
  glew32s.lib
)

if (MSVC)
else()
# Linux -->
target_link_libraries(${PROJECT_NAME} GL GLEW ${CMAKE_DL_LIBS})
endif()
