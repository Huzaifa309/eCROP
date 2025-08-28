cmake_minimum_required(VERSION 3.16)
project(eCROP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the wrapper package (installed to /usr/local or a custom prefix)
find_package(aeronWrapper CONFIG REQUIRED)

# Find libpqxx
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# Collect source files
file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Create a single executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include project includes and lib includes
target_include_directories(${PROJECT_NAME} PRIVATE
    /usr/local/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    aeronWrapper::aeronWrapper
    /usr/local/lib/libquillLogger.a
    /usr/local/lib/libDbFactory.a
    ${PQXX_LIBRARIES}
)
