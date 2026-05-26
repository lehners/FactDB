
# Define the location of the pre-commit hook script
set(PRE_COMMIT_HOOK_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/makeutil/hooks/pre-commit")
set(PRE_COMMIT_HOOK_DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/.git/hooks/pre-commit")

# Function to check for a file's existence
function(file_exists FILEPATH RESULT)
  if(EXISTS ${FILEPATH})
    set(${RESULT} TRUE PARENT_SCOPE)
  else()
    set(${RESULT} FALSE PARENT_SCOPE)
  endif()
endfunction()

# Check if the pre-commit hook already exists
file_exists(${PRE_COMMIT_HOOK_DESTINATION} PRE_COMMIT_HOOK_EXISTS)

# Check if the .git/hooks directory exists
file_exists("${CMAKE_SOURCE_DIR}/.git/hooks" GIT_HOOKS_DIR_EXISTS)

if(NOT PRE_COMMIT_HOOK_EXISTS)
  message(STATUS "No pre-commit hook found, attempting to add symlink")

  if(GIT_HOOKS_DIR_EXISTS)
    # Create the symbolic link
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${PRE_COMMIT_HOOK_SOURCE} ${PRE_COMMIT_HOOK_DESTINATION}
        RESULT_VARIABLE LINK_RESULT
    )
  else()
    message(WARNING "Could not add pre-commit hook, .git/hooks folder not found")
  endif()
endif()

unset(PRE_COMMIT_HOOK_SOURCE)
unset(PRE_COMMIT_HOOK_DESTINATION)
unset(PRE_COMMIT_HOOK_EXISTS)
unset(GIT_HOOKS_DIR_EXISTS)
