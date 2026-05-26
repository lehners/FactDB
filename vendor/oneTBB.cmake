# find_package(TBB REQUIRED)
# find_package(TBB COMPONENTS tbbmalloc tbbmalloc_proxy tbb_preview)

include(FetchContent)

FetchContent_Declare(tbb
    GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
    GIT_TAG v2021.13.0
)

FetchContent_MakeAvailable(tbb)
