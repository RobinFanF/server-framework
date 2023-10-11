cmake_minimum_required(VERSION 3.0)
PROJECT(myserver)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -g -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated  -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -g -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated  -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")


find_library(YAMLCPP libyaml-cpp.a)

set(LIB_SRC
    src/log.cpp
    src/config.cpp
    src/utils.cpp
    src/thread.cpp
    src/fiber.cpp
    src/scheduler.cpp
    src/iomanager.cpp
    src/timer.cpp
    src/fd_manager.cpp
    src/hook.cpp
)

add_library(myserver SHARED ${LIB_SRC})

set(LIB_LIB
    myserver
    pthread
    dl
    ${YAMLCPP}
)

add_executable(test_log "tests/test_log.cpp")
add_dependencies(test_log myserver)
target_link_libraries(test_log myserver ${LIB_LIB})

add_executable(test_config "tests/test_config.cpp")
add_dependencies(test_config myserver)
target_link_libraries(test_config myserver ${LIB_LIB})

add_executable(test_thread "tests/test_thread.cpp")
add_dependencies(test_thread myserver)
target_link_libraries(test_thread myserver ${LIB_LIB})

add_executable(test_utils "tests/test_utils.cpp")
add_dependencies(test_utils myserver)
target_link_libraries(test_utils myserver ${LIB_LIB})

add_executable(test_fiber "tests/test_fiber.cpp")
add_dependencies(test_fiber myserver)
target_link_libraries(test_fiber myserver ${LIB_LIB})

add_executable(test_scheduler "tests/test_scheduler.cpp")
add_dependencies(test_scheduler myserver)
target_link_libraries(test_scheduler myserver ${LIB_LIB})

add_executable(test_iomanager "tests/test_iomanager.cpp")
add_dependencies(test_iomanager myserver)
target_link_libraries(test_iomanager myserver ${LIB_LIB})

add_executable(test_hook "tests/test_hook.cpp")
add_dependencies(test_hook myserver)
target_link_libraries(test_hook myserver ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)