cmake_minimum_required(VERSION 3.19)

set(C74_MIN_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/min-api)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



# Extract package name from directory path.
string(REGEX REPLACE ".*/([^/]+)" "\\1" 
       THIS_PACKAGE_NAME 
       "${CMAKE_CURRENT_SOURCE_DIR}")
project(${THIS_PACKAGE_NAME} VERSION 1.0.0 LANGUAGES C CXX)

if(APPLE)
    if(POLICY CMP0118)
        cmake_policy(SET CMP0118 NEW)
    endif()
endif()

if(MSVC)
    # Use static runtime for both Debug and Release
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    # Or alternatively, set flags directly
    add_compile_options(
        "$<$<CONFIG:Debug>:/MTd>"
        "$<$<CONFIG:Release>:/MT>"
    )
endif()

if(MINGW)
    # Static linking flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -static")
endif()

#############################################################
# Functions
#############################################################

macro(build_info)
    # Build info variables
    set(PROJECT_VERSION "1.0.0")
    string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
    string(TIMESTAMP BUILD_TIME "%H:%M:%S") 
    string(TIMESTAMP BUILD_DATETIME "%Y-%m-%d %H:%M:%S")
endmacro()

macro(project_name)
    # Creates the name of the package from the directory name.
    string(REGEX REPLACE ".*/([^/]+)" "\\1" THIS_PACKAGE_NAME "${CMAKE_CURRENT_SOURCE_DIR}")
    project(${THIS_PACKAGE_NAME} VERSION 1.0.0 LANGUAGES C CXX)
endmacro()

include(${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/ProjectTemplate.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/source/thulr/scripts/cmake/LibraryTemplate.cmake)

# Enable testing framework.
enable_testing()

# Ignore Test Files
list(FILTER SHARED_SOURCES EXCLUDE REGEX ".*_test\\.cpp$")

if (APPLE)
    if (${CMAKE_GENERATOR} MATCHES "Xcode")
            if (${XCODE_VERSION} VERSION_LESS 10)
                message(STATUS "Xcode 10 or higher is required. Please install from the Mac App Store.")
                return ()
            elseif(${XCODE_VERSION} VERSION_GREATER_EQUAL 12)
                set(C74_BUILD_FAT YES)
            endif ()
    endif ()

    if (NOT CMAKE_OSX_ARCHITECTURES)
        if(C74_BUILD_FAT)
            set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "macOS architecture" FORCE)
        else()
            set(CMAKE_OSX_ARCHITECTURES ${CMAKE_SYSTEM_PROCESSOR} CACHE STRING "macOS architecture" FORCE)
        endif()
        message("CMAKE_OSX_ARCHITECTURES set to ${CMAKE_OSX_ARCHITECTURES}")
    endif()
endif()

# Misc setup and subroutines
include(${CMAKE_CURRENT_SOURCE_DIR}/source/min-api/script/min-package.cmake)

# Add the Lib, if it exists
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/min-lib/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/min-lib)
endif ()

#############################################################
# Catch2 - PURE STATIC LIBRARY
#############################################################

# FORCE STATIC RUNTIME FOR CATCH2
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.8 # This version is important. 
                           # Newer releases don't work.
)

# FORCE STATIC LIBRARY
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CATCH_BUILD_STATIC_LIBRARY ON CACHE BOOL "" FORCE)
set(CATCH_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(CATCH_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(Catch2)

# CREATE TARGETS
if(TARGET Catch2)   
    message(STATUS "Catch2 configured as static library with static runtime")
else()
    message(FATAL_ERROR "Catch2 target not found!")
endif()

#############################################################
# CATCH2 TESTS FOR THULR LIBRARIES WITHOUT MAX/MIN
#############################################################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/thulr)

#############################################################
# Max
#############################################################

# Generate a project for every folder in the "source/projects" folder
SUBDIRLIST(PROJECT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/source/projects)
foreach (project_dir ${PROJECT_DIRS})
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir}/CMakeLists.txt")
        message("Generating: ${project_dir}")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir})
    endif ()
endforeach ()


foreach (project_dir ${PROJECT_DIRS})
    if (TARGET ${project_dir}_test)
        target_include_directories(${project_dir}_test PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/source/thulr/source
        )
    endif()
endforeach()
