cmake_minimum_required(VERSION 3.12)

# setup conan profiles
set(PROFILE_PATH_Windows_Release "${CMAKE_CURRENT_SOURCE_DIR}/conanProfiles/windowsRelease")
set(PROFILE_PATH_Windows_Debug "${CMAKE_CURRENT_SOURCE_DIR}/conanProfiles/windowsDebug")
set(PROFILE_PATH_Linux_Release "${CMAKE_CURRENT_SOURCE_DIR}/conanProfiles/linuxRelease")
set(PROFILE_PATH_Linux_Debug "${CMAKE_CURRENT_SOURCE_DIR}/conanProfiles/linuxDebug")

#set(CONAN_DEPLOYER "full_deploy")

set(CURRENT_PROFILE "PROFILE_PATH_${CMAKE_HOST_SYSTEM_NAME}_${CMAKE_BUILD_TYPE}")
message(STATUS "Set conan profile: ${CURRENT_PROFILE}")

if(DEFINED ${CURRENT_PROFILE})
    set(CONAN_PROFILE "${${CURRENT_PROFILE}}")
    message(STATUS "Using conan profile: ${CONAN_PROFILE}")
else()
    message(FATAL_ERROR "Unsupported configuration for: ${CURRENT_PROFILE}")
endif()

include(${CMAKE_SOURCE_DIR}/Automation/CMAKE/CMakeHelpers.cmake)
setup_conan(${CONAN_PROFILE} ${CONAN_PROFILE})

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

set(PROJECT_NAME "ip_filter")

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})

file(WRITE "${CMAKE_BINARY_DIR}/project_info.env" 
     "PROJECT_NAME=${CMAKE_PROJECT_NAME}\n"
     "PROJECT_VERSION=${CMAKE_PROJECT_VERSION}\n"
     "PROJECT_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR}\n"
     "PROJECT_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR}\n"
     "PROJECT_VERSION_PATCH=${CMAKE_PROJECT_VERSION_PATCH}\n")

option(WITH_BOOST_TEST "Whether to build Boost test" ON)
option(WITH_GTEST "Whether to build Google test" ON)

configure_file(version.h.in version.h)

file(GLOB_RECURSE LIB_SOURCES "Source/*.cpp")
file(GLOB_RECURSE LIB_HEADERS "Source/*.h")

add_executable(${PROJECT_NAME}_cli main.cpp)
add_library(${PROJECT_NAME} ${LIB_SOURCES} ${LIB_HEADERS})

set_target_properties(${PROJECT_NAME}_cli ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(${PROJECT_NAME}
    PRIVATE "${CMAKE_BINARY_DIR}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

# Добавляем include директории для main executable
target_include_directories(${PROJECT_NAME}_cli
    PRIVATE "${CMAKE_BINARY_DIR}"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

target_link_libraries(${PROJECT_NAME}_cli PRIVATE
    ${PROJECT_NAME}
)

if(WITH_BOOST_TEST)
    find_package(Boost REQUIRED)
    
    add_executable(test_version test_version.cpp)

    set_target_properties(test_version PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
    )

    target_include_directories(test_version
        PRIVATE "${CMAKE_BINARY_DIR}"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    )

    target_link_libraries(test_version Boost::unit_test_framework ${PROJECT_NAME})
endif()

if(WITH_GTEST)
    find_package(GTest REQUIRED)
    
    add_executable(test_gtest test_gtest.cpp)

    set_target_properties(test_gtest PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
    )

    target_include_directories(test_gtest
        PRIVATE "${CMAKE_BINARY_DIR}"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    )

    target_link_libraries(test_gtest GTest::gtest GTest::gtest_main ${PROJECT_NAME})
endif()

if (MSVC)
    target_compile_options(${PROJECT_NAME}_cli PRIVATE
        /W4
    )
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
    )
    if(WITH_BOOST_TEST)
        target_compile_options(test_version PRIVATE
            /W4
        )
    endif()
    if(WITH_GTEST)
        target_compile_options(test_gtest PRIVATE
            /W4
        )
    endif()
else ()
    target_compile_options(${PROJECT_NAME}_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    if(WITH_BOOST_TEST)
        target_compile_options(test_version PRIVATE
            -Wall -Wextra -pedantic -Werror
        )
    endif()
    if(WITH_GTEST)
        target_compile_options(test_gtest PRIVATE
            -Wall -Wextra -pedantic -Werror
        )
    endif()
endif()

install(TARGETS ${PROJECT_NAME}_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_BOOST_TEST OR WITH_GTEST)
    enable_testing()
endif()

if(WITH_BOOST_TEST)
    add_test(test_version test_version)
endif()

if(WITH_GTEST)
    add_test(test_gtest test_gtest)
endif()
