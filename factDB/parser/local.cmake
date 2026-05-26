# ---------------------------------------------------------------------------
# FACTDB (adapted from imlab)
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Bison & Flex
# ---------------------------------------------------------------------------

# Register flex and bison output
set(PARSER_SCANNER_OUT     "${CMAKE_SOURCE_DIR}/factDB/parser/gen/scanner.cc")
set(PARSER_PARSER_OUT      "${CMAKE_SOURCE_DIR}/factDB/parser/gen/parser.cc")

# Clear the output files
file(WRITE ${PARSER_SCANNER_OUT} "")
file(WRITE ${PARSER_PARSER_OUT} "")

# Generate parser & scanner
add_custom_target(parser
        COMMAND ${BISON_EXECUTABLE}
        --defines="${CMAKE_SOURCE_DIR}/factDB/parser/gen/parser.h"
        --output=${PARSER_PARSER_OUT}
        --report=state
        --report-file="${CMAKE_BINARY_DIR}/parser_bison.log"
        "${CMAKE_SOURCE_DIR}/factDB/parser/parser.y"
        COMMAND ${FLEX_EXECUTABLE}
        --outfile=${PARSER_SCANNER_OUT}
        "${CMAKE_SOURCE_DIR}/factDB/parser/scanner.l"
        DEPENDS "${CMAKE_SOURCE_DIR}/factDB/parser/parser.y"
        "${CMAKE_SOURCE_DIR}/factDB/parser/scanner.l")
add_dependencies(parser fmt::fmt)

execute_process(
    COMMAND git update-index --assume-unchanged ${PARSER_SCANNER_OUT} ${PARSER_PARSER_OUT}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
