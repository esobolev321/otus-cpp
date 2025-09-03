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
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(helloworld VERSION ${PROJECT_VESRION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)

configure_file(version.h.in version.h)

add_executable(helloworld_cli main.cpp)
add_library(helloworld lib.cpp)

set_target_properties(helloworld_cli helloworld PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(helloworld
    PRIVATE "${CMAKE_BINARY_DIR}"
)

target_link_libraries(helloworld_cli PRIVATE
    helloworld
)

if(WITH_BOOST_TEST)
    find_package(Boost REQUIRED)
    
    add_executable(test_version test_version.cpp)

    set_target_properties(test_version PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
    )


    target_link_libraries(test_version Boost::unit_test_framework helloworld)
endif()

if (MSVC)
    target_compile_options(helloworld_cli PRIVATE
        /W4
    )
    target_compile_options(helloworld PRIVATE
        /W4
    )
    if(WITH_BOOST_TEST)
        target_compile_options(test_version PRIVATE
            /W4
        )
    endif()
else ()
    target_compile_options(helloworld_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(helloworld PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    if(WITH_BOOST_TEST)
        target_compile_options(test_version PRIVATE
            -Wall -Wextra -pedantic -Werror
        )
    endif()
endif()

install(TARGETS helloworld_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_BOOST_TEST)
    enable_testing()
    add_test(test_version test_version)
endif()
