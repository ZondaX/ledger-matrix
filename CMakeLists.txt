#*******************************************************************************
#*   (c) 2018 ZondaX GmbH
#*
#*  Licensed under the Apache License, Version 2.0 (the "License");
#*  you may not use this file except in compliance with the License.
#*  You may obtain a copy of the License at
#*
#*      http://www.apache.org/licenses/LICENSE-2.0
#*
#*  Unless required by applicable law or agreed to in writing, software
#*  distributed under the License is distributed on an "AS IS" BASIS,
#*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#*  See the License for the specific language governing permissions and
#*  limitations under the License.
#********************************************************************************
cmake_minimum_required(VERSION 3.0)
project(ledger-matrix VERSION 0.0.0)
enable_testing()

set(CMAKE_CXX_STANDARD 11)
add_subdirectory(cmake/gtest)

string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address -fno-omit-frame-pointer")
string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fsanitize=address -fno-omit-frame-pointer")
#string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=undefined")
#string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fsanitize=undefined")

##############################################################
##############################################################
#  static libs
file(GLOB_RECURSE JSMN_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger/deps/jsmn/src/jsmn.c
        )

file(GLOB_RECURSE LIB_SRC
        src/ledger/src/lib/json_parser.c
        src/ledger/src/lib/tx_parser.c
        src/ledger/src/lib/tx_display.c
        src/ledger/src/lib/tx_validate.c
        )
file(GLOB_RECURSE TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)

add_library(json_parser STATIC ${LIB_SRC} ${JSMN_SRC})
target_include_directories(json_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger/deps/jsmn/src)

add_executable(tests ${TESTS_SRC})
target_link_libraries(tests gtest_main json_parser)

add_test(gtest ${PROJECT_BINARY_DIR}/tests)

###############################################################
# Force tests to depend from app compiling
###############################################################

#set(DISABLE_DOCKER_BUILDS OFF CACHE BOOL "Disables Docker Builds")
#
#if (NOT DISABLE_DOCKER_BUILDS)
#    add_custom_target(ledger_app
#            COMMAND make build
#            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#            )
#    target_include_directories(ledger_app PUBLIC
#            ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger/src
#            ${CMAKE_CURRENT_SOURCE_DIR}/src/ledger/deps/jsmn/src
#            )
#    add_dependencies(tests ledger_app)
#endif()
