cmake_minimum_required(VERSION 3.16)
project(eCROP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch Aeron from GitHub
FetchContent_Declare(
    aeron
    GIT_REPOSITORY https://github.com/aeron-io/aeron.git
    GIT_TAG        master  # or specify a specific version tag like "1.45.0"
)

# Configure Aeron build options
set(BUILD_AERON_DRIVER ON CACHE BOOL "Build Aeron driver")
set(BUILD_AERON_ARCHIVE_API ON CACHE BOOL "Build Aeron archive API")
set(C_WARNINGS_AS_ERRORS OFF CACHE BOOL "Disable warnings as errors")
set(CXX_WARNINGS_AS_ERRORS OFF CACHE BOOL "Disable warnings as errors")

FetchContent_MakeAvailable(aeron)

# Find libpqxx
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# Collect source files
file(GLOB SOURCE_FILES "src/*.cpp")

# Create a single eCROP executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Include Aeron headers and project includes
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    /usr/local/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    /usr/local/lib/libaeronWrapper.a
    /usr/local/lib/libquillLogger.a
    /usr/local/lib/libDbFactory.a
    aeron_client
    aeron_driver
    ${PQXX_LIBRARIES}
)
