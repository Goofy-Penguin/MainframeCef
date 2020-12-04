cmake_minimum_required (VERSION 3.8)

include(FetchContent)

# version
set(MAINFRAME_CEF_VERSION_MAJOR 0 CACHE STRING "major version" FORCE)
set(MAINFRAME_CEF_VERSION_MINOR 1 CACHE STRING "minor version" FORCE)
set(MAINFRAME_CEF_VERSION ${MAINFRAME_CEF_VERSION_MAJOR}.${MAINFRAME_CEF_VERSION_MINOR} CACHE STRING "version" FORCE)
set(MAINFRAME_CEF_BRANCH_MAINFRAME "master" CACHE STRING "the branch used for mainframe")

# flags
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSPDLOG_FMT_EXTERNAL")

# libraries
set(build_tests FALSE)
FetchContent_GetProperties(mainframe)
if(NOT mainframe_POPULATED)
	FetchContent_Declare(mainframe
		GIT_REPOSITORY https://github.com/Goofy-Penguin/Mainframe.git
		GIT_TAG origin/${MAINFRAME_CEF_BRANCH_MAINFRAME}
	)

	FetchContent_MakeAvailable(mainframe)
endif()

include("cmake/HunterGate.cmake")
HunterGate(
	URL "https://github.com/cpp-pm/hunter/archive/v0.23.254.tar.gz"
	SHA1 "017e688b9848c1f695303e8cfe9f35d40e154112"
)


project ("mainframeCEF" VERSION ${MAINFRAME_CEF_VERSION})

hunter_add_package(fmt)
hunter_add_package(nlohmann_json)

find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)

set(CEF_VERSION 87.1.11+g8bb7705+chromium-87.0.4280.66)
include("cmake/cef.cmake")
FetchContent_Declare(cefdll_wrapper GIT_REPOSITORY https://github.com/edunad/cef-cmake.git)
FetchContent_MakeAvailable(cefdll_wrapper)

if (WIN32)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")
endif()

file(GLOB_RECURSE source_files
	"code/src/*.cpp"
	"code/src/*.c"
	"code/include/*.hpp"
)

set(output_target mainframe.cef)
add_library(${output_target} ${source_files})
target_include_directories(${output_target} PUBLIC "code/include")
target_compile_features(${output_target} PRIVATE cxx_std_17)
target_link_libraries(${output_target} PRIVATE mainframe.ui freetype-gl ${mf_winlibs} PUBLIC cefdll_wrapper mainframe.utils mainframe.render mainframe.math)
set_target_properties(${output_target}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
)

add_subdirectory("tests")
