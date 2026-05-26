find_package(GTest)

# if (NOT GTest_FOUND)
#     message(STATUS "Adding bundled Google Test")
#     set(BUILD_GMOCK OFF CACHE BOOL INTERNAL)
#     set(INSTALL_GTEST OFF CACHE BOOL INTERNAL)
#
#     add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/googletest)
#
#     add_library(GTest::GTest ALIAS gtest)
# endif ()


# =========================== or

IF (NOT GTest_FOUND)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY  https://github.com/google/googletest.git
        GIT_TAG         v1.14.0
        GIT_PROGRESS    TRUE
        EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(googletest)

ENDIF ()
