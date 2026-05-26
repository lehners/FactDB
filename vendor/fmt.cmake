
include(FetchContent)

FetchContent_Declare(fmt
    GIT_REPOSITORY   "https://github.com/fmtlib/fmt.git"
    GIT_TAG          10.1.0
    GIT_PROGRESS     TRUE
    EXCLUDE_FROM_ALL
    )

FetchContent_MakeAvailable(fmt)
