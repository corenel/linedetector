cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_definitions(-Wall -Wno-dev -O2)

project(line_detection)

if (APPLE)
    # Fix linking on macOS 10.14+. See https://stackoverflow.com/questions/54068035
    link_directories(/usr/local/lib)
    include_directories(/usr/local/include)
endif ()

include(cmake_modules/util.cmake)

# OpenCV
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# gflags
LIST(APPEND SEARCH_LIBS /usr/lib)
LIST(APPEND SEARCH_LIBS /usr/local/lib)
LIST(APPEND SEARCH_LIBS /opt/local/lib)
LIST(APPEND SEARCH_HEADERS /usr/include)
LIST(APPEND SEARCH_HEADERS /usr/local/include)
LIST(APPEND SEARCH_HEADERS /opt/local/include)
find_library(GFLAGS_LIB NAMES gflags PATHS ${SEARCH_LIBS})
find_path(GFLAGS_INCLUDE NAMES gflags/gflags.h PATHS ${SEARCH_HEADERS})

set(GOOGLE_LIBRARIES ${GFLAGS_LIB})

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
    ${GFLAGS_INCLUDE}
)

add_library(line_detector src/line_detector.cpp)
target_link_libraries(line_detector ${OpenCV_LIBS})

add_executable(line_detection src/main.cpp)
target_link_libraries(line_detection ${GOOGLE_LIBRARIES} line_detector)

OPTION(BUILD_PYTHON_BINDINGS "Build bindings for Python" OFF)
if (BUILD_PYTHON_BINDINGS)
    # find swig
    find_package(SWIG REQUIRED)
    set(CMAKE_SWIG_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/../opencv-swig/lib/")
    include(${SWIG_USE_FILE})
    # find python library
    set(TEST_PYTHON_VERSION "3"
        CACHE STRING "Python version used on tests.")
    set_property(CACHE TEST_PYTHON_VERSION PROPERTY STRINGS "3" "2")
    if (TEST_PYTHON_VERSION EQUAL 2)
        find_package(PythonLibs 2 REQUIRED)
    else ()
        find_package(PythonLibs 3 REQUIRED)
    endif ()
    message(STATUS "Python: ${TEST_PYTHON_VERSION}")
    # include
    include_directories(
        bindings
        ${PYTHON_INCLUDE_DIRS}
    )
    get_property(_include_directories DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
    foreach (_include_directory IN LISTS _include_directories)
        list(APPEND CMAKE_SWIG_FLAGS -I${_include_directory})
    endforeach ()
    # build bindings
    add_python_binding(line_detector)
endif (BUILD_PYTHON_BINDINGS)
