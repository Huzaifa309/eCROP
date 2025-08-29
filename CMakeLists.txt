cmake_minimum_required(VERSION 3.16)
project(eCrop VERSION 1.0.0 LANGUAGES CXX)

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

# --- quillLogger library ---
add_library(quillLogger STATIC IMPORTED)
set_target_properties(quillLogger PROPERTIES
    IMPORTED_LOCATION "/usr/local/lib/libquillLogger.a"
    INTERFACE_INCLUDE_DIRECTORIES "/usr/local/include"
)

# --- DbFactory library ---
add_library(DbFactory STATIC IMPORTED)
set_target_properties(DbFactory PROPERTIES
    IMPORTED_LOCATION "/usr/local/lib/libDbFactory.a"
    INTERFACE_INCLUDE_DIRECTORIES "/usr/local/include"
)

# --- Link everything ---
target_link_libraries(${PROJECT_NAME} PRIVATE
    aeronWrapper::aeronWrapper
    quillLogger
    DbFactory
    ${PQXX_LIBRARIES}
)
