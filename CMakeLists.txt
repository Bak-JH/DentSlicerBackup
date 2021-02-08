cmake_minimum_required(VERSION 3.19.3)
#functions
#https://stackoverflow.com/questions/60211516/programmatically-get-all-targets-in-a-cmake-project
# Collect all currently added targets in all subdirectories
#
# Parameters:
# - _result the list containing all found targets
# - _dir root directory to start looking from
function(get_all_targets _result _dir)
    get_property(_subdirs DIRECTORY "${_dir}" PROPERTY SUBDIRECTORIES)
    foreach(_subdir IN LISTS _subdirs)
        message(STATUS "searching sub: ${_subdir}")
        get_all_targets(${_result} "${_subdir}")
    endforeach()

    get_directory_property(_sub_targets DIRECTORY "${_dir}" BUILDSYSTEM_TARGETS)
    set(${_result} ${${_result}} ${_sub_targets} PARENT_SCOPE)
endfunction()



function(get_all_subdir _result _dir)
    message(STATUS "test: ${_dir}")
    get_property(_subdirs DIRECTORY "${_dir}" PROPERTY SUBDIRECTORIES)

    foreach(_subdir IN LISTS _subdirs)
        message(STATUS "sub: ${_subdir}")
        # get_all_targets(${_result} "${_subdir}")
    endforeach()
endfunction()

#Platforms
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

#need pkg-config, for LZ4, flann
find_package(PkgConfig REQUIRED)
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH TRUE)


#find-modules
# list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
# list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR})
# list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/lz4/lib)

message(STATUS "all find modules: ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

#submodules
message(STATUS "lz4 adding subdirectory")


# get_all_subdir(test "${CMAKE_CURRENT_SOURCE_DIR}")
# get_all_targets(allTargets "${CMAKE_CURRENT_SOURCE_DIR}")
# message(STATUS "all targets yeeees: ${allTargets}")
# message(STATUS "all targets: ${BUILDSYSTEM_TARGETS}")
set(LZ4_BUNDLED_MODE OFF)
add_subdirectory(extern/lz4/build/cmake)
if(NOT DEFINED LZ4_ROOT_DIR)
  set(LZ4_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/lz4)
endif()
# list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/extern/lz4/build/cmake)
set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH}:${CMAKE_CURRENT_SOURCE_DIR}/build/extern/lz4/build/cmake")
message(STATUS "lz4 adding subdirectory finished")
message(STATUS "boost adding subdirectory")
add_subdirectory(extern/boost)
message(STATUS "eigen adding subdirectory")
add_subdirectory(extern/eigen)
message(STATUS "flann adding subdirectory")
add_subdirectory(extern/flann)
message(STATUS "vtk adding subdirectory")
add_subdirectory(extern/vtk)
message(STATUS "cork adding subdirectory")
add_subdirectory(extern/cork)
message(STATUS "glm adding subdirectory")
add_subdirectory(extern/glm)
message(STATUS "rapidjson adding subdirectory")
add_subdirectory(extern/rapidjson)
message(STATUS "taskflow adding subdirectory")
add_subdirectory(extern/taskflow)
message(STATUS "frozenString adding subdirectory")
add_subdirectory(extern/frozenString)
message(STATUS "readerwriterqueue adding subdirectory")
add_subdirectory(extern/readerwriterqueue)
add_subdirectory(src)
message(STATUS "dent slicer done")







