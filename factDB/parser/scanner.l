%{
// ---------------------------------------------------------------------------------------------------
// FACTDB (adapted from IMLAB)
// ---------------------------------------------------------------------------------------------------
%}

%{
// ---------------------------------------------------------------------------------------------------
// Header
// ---------------------------------------------------------------------------------------------------
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>
#include <istream>
#include "factDB/parser/SchemaParseContext.hpp"
#include "factDB/parser/AST.hpp"
#include "./parser.h"

namespace factDB::parser {

// The location of the current token
extern factDB::parser::location loc;
// The input stream of the scanner
extern std::istream *in;

}  // namespace factDB::parser

using namespace factDB::parser;

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.
#undef yywrap
#define yywrap() 1

// Declare the yylex function
#define YY_DECL SchemaParser::symbol_type yylex(SchemaParseContext& sc)
// Configure the scanner to use istreams
#define YY_INPUT(buffer, result, max_size)  \
    result = 0; \
    while (true) { \
        int c = in->get(); \
        if (in->eof()) break; \
        buffer[result++] = c; \
        if (result == max_size || c == '\n') break; \
    }
#if __cplusplus > 199711L
    #define register      // Deprecated in C++11.
#endif  // #if __cplusplus > 199711L
%}

%{
// ---------------------------------------------------------------------------------------------------
// Options
// ---------------------------------------------------------------------------------------------------
%}
%{
// noyywrap:    Disable yywrap (EOF == end of parsing)
// nounput:     Disable manipulation of input stream
// noinput:     Disable explicit fetch of the next character
// batch:       Scanner in batch-mode (vs. interactive)
// debug:       Write debug info to stderr
// caseless:    Case-insensitive pattern matching
%}
%option noyywrap
%option nounput
%option noinput
%option batch
%option debug
%option caseless

%{
// Code run each time a token is matched.
// We just update the location of the token.
#define YY_USER_ACTION  { loc.columns(yyleng); }
%}

%%

%{
// Code runs each time yylex is called.
// Set the beginning of the token to the end of the previous token.
loc.step ();
%}

[ \t]+              { loc.step(); }
"\n"                { loc.lines (yyleng); loc.step (); }
";"                 { return SchemaParser::make_SEMICOLON(loc); }
","                 { return SchemaParser::make_COMMA(loc); }
"{"                 { return SchemaParser::make_LCB(loc); }
"}"                 { return SchemaParser::make_RCB(loc); }
"("                 { return SchemaParser::make_LRB(loc); }
")"                 { return SchemaParser::make_RRB(loc); }
"integer"           { return SchemaParser::make_INTEGER(loc); }
"int"               { return SchemaParser::make_INT(loc); }
"uint64"            { return SchemaParser::make_UINT64(loc); }
"char"              { return SchemaParser::make_CHAR(loc); }
"varchar"           { return SchemaParser::make_VARCHAR(loc); }
"timestamp"         { return SchemaParser::make_TIMESTAMP(loc); }
"date"              { return SchemaParser::make_DATE(loc); }
"numeric"           { return SchemaParser::make_NUMERIC(loc); }
"create"            { return SchemaParser::make_CREATE(loc); }
"table"             { return SchemaParser::make_TABLE(loc); }
"index"             { return SchemaParser::make_INDEX(loc); }
"not"               { return SchemaParser::make_NOT(loc); }
"null"              { return SchemaParser::make_NULL(loc); }
"on"                { return SchemaParser::make_ON(loc); }
"."                 { return SchemaParser::make_DOT(loc); }
"primary"           { return SchemaParser::make_PRIMARY(loc); }
"select"            { return SchemaParser::make_SELECT(loc); }
"from"              { return SchemaParser::make_FROM(loc); }
"where"             { return SchemaParser::make_WHERE(loc); }
"key"               { return SchemaParser::make_KEY(loc); }
"+"                 { return SchemaParser::make_PLUS(loc); }
"-"                 { return SchemaParser::make_MINUS(loc); }
"/"                 { return SchemaParser::make_SOLIDUS(loc); }
"^"                 { return SchemaParser::make_POW(loc); }
"%"                 { return SchemaParser::make_MOD(loc); }
"="                 { return SchemaParser::make_EQ(loc); }
"!="                { return SchemaParser::make_NEQ1(loc); }
"<>"                { return SchemaParser::make_NEQ2(loc); }
"<"                 { return SchemaParser::make_LT(loc); }
">"                 { return SchemaParser::make_GT(loc); }
"<="                { return SchemaParser::make_LE(loc); }
">="                { return SchemaParser::make_GE(loc); }
"min"               { return SchemaParser::make_MIN(loc); }
"max"               { return SchemaParser::make_MAX(loc); }
"avg"               { return SchemaParser::make_AVG(loc); }
"count"             { return SchemaParser::make_COUNT(loc); }
"sum"               { return SchemaParser::make_SUM(loc); }
"and"               { return SchemaParser::make_AND(loc); }
"or"                { return SchemaParser::make_OR(loc); }
"between"           { return SchemaParser::make_BETWEEN(loc); }
"is"                { return SchemaParser::make_IS(loc); }
"true"              { return SchemaParser::make_TRUE(loc); }
"false"             { return SchemaParser::make_FALSE(loc); }
"group"             { return SchemaParser::make_GROUP(loc); }
"distinct"          { return SchemaParser::make_DISTINCT(loc); }
"by"                { return SchemaParser::make_BY(loc); }
"having"            { return SchemaParser::make_HAVING(loc); }
"as"                { return SchemaParser::make_AS(loc); }
"*"                 { return SchemaParser::make_ASTERISK(loc); }
"unordered_map"     { return SchemaParser::make_UNORDERED_MAP(loc); }
"ordered_map"       { return SchemaParser::make_ORDERED_MAP(loc); }
"btree_map"         { return SchemaParser::make_BTREE_MAP(loc); }
"with"              { return SchemaParser::make_WITH(loc); }
"copy"              { return SchemaParser::make_COPY(loc); }
"drop"              { return SchemaParser::make_DROP(loc); }
"if"                { return SchemaParser::make_IF(loc); }
"exists"            { return SchemaParser::make_EXISTS(loc); }
"delimiter"         { return SchemaParser::make_DELIMITER(loc); }
"insert"            { return SchemaParser::make_INSERT(loc); }
"into"              { return SchemaParser::make_INTO(loc); }
"values"            { return SchemaParser::make_VALUES(loc); }
[a-zA-Z][a-zA-Z0-9_]*     { return SchemaParser::make_IDENT(yytext, loc); }
\"(\\\"|[^\"])+\"     { return SchemaParser::make_QUOTED_IDENT(yytext, loc); }
\'(\\\'|[^\'])*\'          { return SchemaParser::make_STRING_LITERAL(yytext, loc); }
[.][0-9]+           { return SchemaParser::make_NUMBER(yytext, loc); }
[0-9]+[.][0-9]*     { return SchemaParser::make_NUMBER(yytext, loc); }
[0-9]+              { return SchemaParser::make_INTEGER_VALUE(yytext, loc); }
<<EOF>>             { return SchemaParser::make_EOF(loc); }
.                   { sc.Error(loc.begin.line, loc.begin.column, "invalid character"); }

%%

// ---------------------------------------------------------------------------------------------------
// Code  [0-9][.]?[a-z0-9_]*     { return SchemaParser::make_IDENTIFIERSELECT(yytext, loc); }
// ---------------------------------------------------------------------------------------------------

// The input stream
factDB::parser::location factDB::parser::loc;
// The input stream of the scanner
std::istream *factDB::parser::in = nullptr;

// Begin a scan
void factDB::parser::SchemaParseContext::beginScan(std::istream &is) {
    yy_init_globals();
    yy_flex_debug = trace_scanning_;
    in = &is;
}

// End a scan
void factDB::parser::SchemaParseContext::endScan() {
    yylex_destroy();
    in = nullptr;
}

