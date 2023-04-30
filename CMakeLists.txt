cmake_minimum_required(VERSION 3.17)
project(maiauth VERSION 1.0 DESCRIPTION "When in doubt")

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(PkgConfig REQUIRED)
pkg_search_module(dpp REQUIRED dpp)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git GIT_TAG 32fd1453194c37d5136577ee55629b863a42333f) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

if (USING_DOCKER MATCHES TRUE)
    link_libraries("-static")
endif ()

target_link_libraries(${PROJECT_NAME} dpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${DPP_INCLUDE_DIR}) 
target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} cpr::cpr)
target_link_libraries(${PROJECT_NAME} cpr::cpr)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)