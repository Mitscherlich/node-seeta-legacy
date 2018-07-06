cmake_minimum_required(VERSION 3.1)

project(seeta VERSION 0.2.0)

set(DEPS_INCLUDE_DIR deps/seetaface2)
set(DEPS_LIB_DIR deps/seetaface2)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(DEPS_INCLUDE_DIR "${DEPS_INCLUDE_DIR}/win32")
  set(DEPS_LIB_DIR "${DEPS_LIB_DIR}/win32")

  if (NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
    set(DEPS_INCLUDE_DIR "${DEPS_INCLUDE_DIR}/x86/include")
    set(DEPS_LIB_DIR "${DEPS_LIB_DIR}/x86/lib")
  elseif (CMAKE_SIZEOF_VOID_P)
    set(DEPS_INCLUDE_DIR "${DEPS_INCLUDE_DIR}/x86/include")
    set(DEPS_LIB_DIR "${DEPS_LIB_DIR}/x86/lib")
  elseif (NOT CMAKE_CL_64)
    set(DEPS_INCLUDE_DIR "${DEPS_INCLUDE_DIR}/x86/include")
    set(DEPS_LIB_DIR "${DEPS_LIB_DIR}/x86/lib")
  else()
    set(DEPS_INCLUDE_DIR "${DEPS_INCLUDE_DIR}/x64/include")
    set(DEPS_LIB_DIR "${DEPS_LIB_DIR}/x64/lib")
  endif()
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(DEPS_INCLUDE_DIR "${DEPS_INCLUDE_DIR}/linux/x86_64/include")
  set(DEPS_LIB_DIR "${DEPS_LIB_DIR}/linux/x86_64/lib")
else()
  message(FATAL_ERROR "Other platform: ${CMAKE_SYSTEM_NAME} is not support.")
endif()

include(cmake/NodeJS.cmake)

nodejs_init()

include_directories("${DEPS_INCLUDE_DIR}")
link_directories("${DEPS_LIB_DIR}")
set(SOURCE
  src/seeta.cpp
  src/face-detector.cpp
  src/point-detector.cpp
  src/face-recognizer.cpp)

add_nodejs_module(seeta ${SOURCE})

find_package(OpenCV REQUIRED)

if(CMAKE_VERSION VERSION_LESS "2.8.11")
  include_directories(${OpenCV_INCLUDE_DIRS})
endif()

set(SEETA_REQUIRED_LIB ${OpenCV_LIBS})

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_custom_command(TARGET seeta POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${DEPS_LIB_DIR}"
      $<TARGET_FILE_DIR:seeta>)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
  list(APPEND SEETA_REQUIRED_LIB
    SeetaFaceDetector200
    SeetaFaceRecognizer200
    SeetaPointDetector200
    holiday)
endif()

target_link_libraries(seeta ${SEETA_REQUIRED_LIB})
