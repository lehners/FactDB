include(FetchContent)

# download library
FetchContent_Declare(kuzu
    URL "https://github.com/kuzudb/kuzu/releases/download/v0.1.0/libkuzu-linux-x86_64.zip"
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

FetchContent_Populate(kuzu)

# add headers
include_directories(SYSTEM "${CMAKE_CURRENT_BINARY_DIR}/_deps/kuzu-src")

# use alias for library
add_library( kuzulib SHARED IMPORTED )
set_target_properties( kuzulib PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/_deps/kuzu-src/libkuzu.so" )
target_compile_definitions(kuzulib INTERFACE -D_GLIBCXX_USE_CXX11_ABI=0)



include(FetchContent)

FetchContent_Declare(kuzu_git
    GIT_REPOSITORY   "https://github.com/kuzudb/kuzu.git"
    GIT_TAG          v0.1.0
    EXCLUDE_FROM_ALL
    # GIT_PROGRESS     TRUE
)

FetchContent_MakeAvailable(kuzu_git)

