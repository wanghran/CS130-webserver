cmake_minimum_required(VERSION 3.10.0)
project(hungrynerds-assignment-2)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system log)
message(STATUS "Boost version: ${Boost_VERSION}")

# Enable SQLite3
# https://dmerej.info/blog/post/chuck-norris-part-2-sqlite-conan/
add_library(sqlite3 STATIC IMPORTED)
set_target_properties(sqlite3
  PROPERTIES
  IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libsqlite3.a
  INTERFACE_INCLUDE_DIRECTORIES /usr/include
)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set_target_properties(sqlite3
    PROPERTIES
      INTERFACE_LINK_LIBRARIES "dl;pthread"
   )
endif()

include_directories(include)

# TODO(!): Update name and srcs
add_library(logger_lib src/logger.cc)
add_library(request_lib src/request.cc)
add_library(session_lib src/session.cc)
add_library(server_lib src/server.cc)
#add_library(config_parser_lib src/config_parser.cc)
add_library(NginxConfig_lib src/NginxConfig.cc)
add_library(NginxConfigParser_lib src/NginxConfigParser.cc)
add_library(NginxConfigStatement_lib src/NginxConfigStatement.cc)
add_library(DataProcessor_lib src/DataProcessor.cc)
add_library(UrlProcessor_lib src/UrlProcessor.cc)
add_library(echoHandler_lib src/echoHandler.cc)
add_library(staticHandler_lib src/staticHandler.cc)
add_library(statusHandler_lib src/statusHandler.cc)
add_library(proxyHandler_lib src/proxyHandler.cc)
add_library(invalidHandler_lib src/invalidHandler.cc)
add_library(formHandler_lib src/formHandler.cc)
add_library(mimeHandler_lib src/mimeHandler.cc)
add_library(HandlerManager_lib src/HandlerManager.cc)
add_library(header_lib src/header.cc)
add_library(header_builder_lib src/header_builder.cc)
add_library(reply_lib src/reply.cc)
add_library(healthHandler_lib src/healthHandler.cc)
add_library(malformHandler_lib src/malformHandler.cc)

# TODO(!): Update executable name, srcs, and deps
add_executable(server src/server_main.cc src/server.cc src/session.cc 
               src/NginxConfig.cc src/NginxConfigParser.cc src/NginxConfigStatement.cc 
               src/DataProcessor.cc src/UrlProcessor.cc src/echoHandler.cc 
	       src/staticHandler.cc src/statusHandler.cc src/proxyHandler.cc src/invalidHandler.cc src/formHandler.cc src/mimeHandler.cc src/logger.cc 
               src/header.cc src/reply.cc src/request.cc src/HandlerManager.cc src/healthHandler.cc src/malformHandler.cc)
target_link_libraries(server Boost::system)
target_link_libraries(server_lib Boost::system)
target_link_libraries(session_lib Boost::system)
target_link_libraries(proxyHandler_lib Boost::system)
target_link_libraries(server Boost::log_setup)
target_link_libraries(server_lib Boost::log_setup)
target_link_libraries(session_lib Boost::log_setup)
target_link_libraries(logger_lib Boost::log_setup)
target_link_libraries(UrlProcessor_lib Boost::log_setup)
target_link_libraries(server Boost::log)
target_link_libraries(server_lib Boost::log)
target_link_libraries(session_lib Boost::log)
target_link_libraries(logger_lib Boost::log)
target_link_libraries(UrlProcessor_lib Boost::log)
target_link_libraries(server sqlite3)
target_link_libraries(server_lib sqlite3)
target_link_libraries(NginxConfig_lib sqlite3)
target_link_libraries(session_lib sqlite3)
#target_link_libraries(config_parser_lib Boost::system)

# TODO(!): Update test executable name, srcs, and deps
add_executable(NginxConfig_test tests/NginxConfig_test.cc)
add_executable(NginxConfigParser_test tests/NginxConfigParser_test.cc)
add_executable(DataProcessor_test tests/DataProcessor_test.cc)
add_executable(UrlProcessor_test tests/UrlProcessor_test.cc)
add_executable(echoHandler_test tests/echoHandler_test.cc)
add_executable(staticHandler_test tests/staticHandler_test.cc)
add_executable(statusHandler_test tests/statusHandler_test.cc)
add_executable(proxyHandler_test tests/proxyHandler_test.cc)
add_executable(invalidHandler_test tests/invalidHandler_test.cc)
add_executable(header_test tests/header_test.cc)
add_executable(reply_test tests/reply_test.cc)
add_executable(healthHandler_test tests/healthHandler_test.cc)
add_executable(malformHandler_test tests/malformHandler_test.cc)
add_executable(mimeHandler_test tests/mimeHandler_test.cc)
add_executable(HandlerManager_test tests/HandlerManager_test.cc)

#target_link_libraries(server_test server_lib session_lib NginxConfig_lib NginxConfigParser_lib NginxConfigStatement_lib gtest_main)
target_link_libraries(NginxConfig_test NginxConfig_lib NginxConfigParser_lib NginxConfigStatement_lib gtest_main)
target_link_libraries(NginxConfigParser_test NginxConfig_lib NginxConfigParser_lib NginxConfigStatement_lib gtest_main)
target_link_libraries(DataProcessor_test DataProcessor_lib request_lib gtest_main)
target_link_libraries(UrlProcessor_test UrlProcessor_lib logger_lib request_lib gtest_main)
target_link_libraries(echoHandler_test echoHandler_lib request_lib NginxConfig_lib reply_lib header_lib gtest_main)
target_link_libraries(staticHandler_test staticHandler_lib request_lib NginxConfig_lib reply_lib header_lib gtest_main)
target_link_libraries(statusHandler_test statusHandler_lib request_lib NginxConfig_lib reply_lib header_lib gtest_main)
target_link_libraries(proxyHandler_test proxyHandler_lib request_lib NginxConfig_lib reply_lib header_lib gtest_main)
target_link_libraries(invalidHandler_test invalidHandler_lib request_lib NginxConfig_lib reply_lib header_lib gtest_main)
target_link_libraries(healthHandler_test healthHandler_lib request_lib NginxConfig_lib reply_lib header_lib gtest_main)
target_link_libraries(malformHandler_test malformHandler_lib request_lib NginxConfig_lib reply_lib header_lib gtest_main)
target_link_libraries(mimeHandler_test mimeHandler_lib request_lib NginxConfig_lib reply_lib header_lib gtest_main)
target_link_libraries(header_test header_lib gtest_main)
target_link_libraries(reply_test reply_lib header_lib gtest_main)
target_link_libraries(HandlerManager_test HandlerManager_lib echoHandler_lib staticHandler_lib statusHandler_lib 
                      proxyHandler_lib invalidHandler_lib formHandler_lib mimeHandler_lib healthHandler_lib 
                      malformHandler_lib request_lib NginxConfig_lib 
                      reply_lib header_lib gtest_main)

# TODO(!): Update with test binary
gtest_discover_tests(NginxConfig_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(NginxConfigParser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(DataProcessor_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(UrlProcessor_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echoHandler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(staticHandler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(statusHandler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(proxyHandler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(invalidHandler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(healthHandler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(malformHandler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(mimeHandler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(header_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(reply_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(HandlerManager_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# TODO(!): Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server session_lib server_lib NginxConfig_lib
                            NginxConfigParser_lib NginxConfigStatement_lib
                            DataProcessor_lib UrlProcessor_lib logger_lib
          invalidHandler_lib staticHandler_lib echoHandler_lib statusHandler_lib proxyHandler_lib
          healthHandler_lib malformHandler_lib formHandler_lib mimeHandler_lib HandlerManager_lib
                            reply_lib header_lib request_lib TESTS NginxConfig_test
                            NginxConfigParser_test DataProcessor_test UrlProcessor_test 
          echoHandler_test staticHandler_test statusHandler_test proxyHandler_test invalidHandler_test
          healthHandler_test malformHandler_test mimeHandler_test HandlerManager_test
                            header_test reply_test)

find_program (BASH_PROGRAM bash)
if (BASH_PROGRAM)
   add_test (CITest ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/integration.sh)
endif (BASH_PROGRAM)
