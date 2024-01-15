cmake_minimum_required(VERSION 3.20)

project(collie C)

add_subdirectory(external)

set(TARGET_NAME collie)

set(SOURCE
  #arrow.c
  #arrow.h

  camera.c
  camera.h

  collision.c
  collision.h

  geometry.c
  geometry.h

  input.c
  input.h

  level.c
  level.h

  main.c

  player.c
  player.h

  shader.c
  shader.h

  texture.c
  texture.h

  window.c
  window.h

  #shaders/arrow.vert.glsl
  #shaders/arrow.frag.glsl

  shaders/level.vert.glsl
  shaders/level.frag.glsl

  shaders/player.vert.glsl
  shaders/player.frag.glsl
)

set(DEPENDENCIES
  assimp
  cglm
  glad
  glfw
  stb
)

if(WIN32)
  set(EXTRA_BINS
    "${CMAKE_SOURCE_DIR}/external/assimp/lib/win/assimp.dll"
    "${CMAKE_SOURCE_DIR}/external/glfw/lib/win/glfw3.dll"
  )
endif()

add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE ${SOURCE})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${TARGET_NAME} PRIVATE ${DEPENDENCIES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE})
set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET_NAME}>")

# Copy extra binaries
foreach (EXTRA_BIN ${EXTRA_BINS})
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${EXTRA_BIN} "$<TARGET_FILE_DIR:${TARGET_NAME}>")
endforeach()

# Copy assets
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/levels" "$<TARGET_FILE_DIR:${TARGET_NAME}>/levels")
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/objects" "$<TARGET_FILE_DIR:${TARGET_NAME}>/objects")
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:${TARGET_NAME}>/shaders")
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/textures" "$<TARGET_FILE_DIR:${TARGET_NAME}>/textures")