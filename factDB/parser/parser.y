// ---------------------------------------------------------------------------------------------------
// FACTDB (adapted from imlab)
// ---------------------------------------------------------------------------------------------------
%skeleton "lalr1.cc"
%require "3.0.4"
// ---------------------------------------------------------------------------------------------------
// Write a parser header file
%defines
// Define the parser class name
%define api.parser.class {SchemaParser}
// Create the parser in our namespace
%define api.namespace { factDB::parser }
// Use C++ variant to store the values and get better type warnings (compared to "union")
%define api.value.type variant
// With variant-based values, symbols are handled as a whole in the scanner
%define api.token.constructor
// Prefix all tokens
%define api.token.prefix {SCHEMA_}
// Check if variants are constructed and destroyed properly
%define parse.assert
// Trace the parser
%define parse.trace
// Use verbose parser errors
%define parse.error verbose
// Enable location tracking.
%locations
// Pass the compiler as parameter to yylex/yyparse.
%param { factDB::parser::SchemaParseContext &sc }
// ---------------------------------------------------------------------------------------------------
// Added to the header file and parser implementation before bison definitions.
// We include string for string tokens and forward declare the SchemaParseContext.
%code requires {
#include <string>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>
#include <istream>
#include <memory>

#include "factDB/schemac/Type.hpp"
#include "factDB/schemac/Table.hpp"
#include "factDB/schemac/IndexType.hpp"
#include "factDB/parser/AST.hpp"
#include "factDB/parser/SchemaParseContext.hpp"
#include "factDB/util/StringUtils.hpp"
// #include "imlab/cts/parser/ParseContext.h"
// #include "imlab/cts/infra/Types.h"
// #include "imlab/cts/infra/Helper.h"
}
// ---------------------------------------------------------------------------------------------------
// Import the compiler header in the implementation file
%code {
factDB::parser::SchemaParser::symbol_type yylex(factDB::parser::SchemaParseContext& sc);
}
// ---------------------------------------------------------------------------------------------------
// Token definitions
%token <std::string>    INTEGER_VALUE          "integer_value"
%token <std::string>    IDENT                  "identifier"
%token <std::string>    QUOTED_IDENT           "quoted_identifier"
%token <std::string>    STRING_LITERAL         "string_literal"
%token <std::string>    NUMBER                 "number"
%token SEMICOLON        "semicolon"
%token COMMA            "comma"
%token LCB              "left_curly_brackets"
%token RCB              "right_curly_brackets"
%token LRB              "left_round_brackets"
%token RRB              "right_round_brackets"
%token INT              "int"
%token INTEGER          "integer"
%token UINT64           "uint64"
%token CHAR             "char"
%token VARCHAR          "varchar"
%token TIMESTAMP        "timestamp"
%token DATE             "date"
%token NUMERIC          "numeric"
%token CREATE           "create"
%token TABLE            "table"
%token INDEX            "index"
%token NOT              "not"
%token NULL             "null"
%token ON               "on"
%token DOT              "."
%token PRIMARY          "primary"
%token SELECT           "select"
%token FROM             "from"
%token WHERE            "where"
%token KEY              "key"
%token PLUS             "+"
%token MINUS            "-"
%token SOLIDUS          "/"
%token POW              "^"
%token MOD              "%"
%token EQ               "="
%token NEQ1             "!="
%token NEQ2             "<>"
%token LT               "<"
%token GT               ">"
%token LE               "<="
%token GE               ">="
%token MIN              "min"
%token MAX              "max"
%token AVG              "avg"
%token COUNT            "count"
%token SUM              "sum"
%token BETWEEN          "between"
%token AND              "and"
%token OR               "or"
%token IS               "is"
%token TRUE             "true"
%token FALSE            "false"
%token GROUP            "group"
%token DISTINCT         "distinct"
%token BY               "by"
%token HAVING           "having"
%token AS               "as"
%token ASTERISK         "asterisk"
%token UNORDERED_MAP    "unordered_map"
%token ORDERED_MAP      "ordered_map"
%token BTREE_MAP        "btree_map"
%token WITH             "with"
%token COPY             "copy"
%token DROP             "drop"
%token IF               "if"
%token EXISTS           "exists"
%token DELIMITER        "delimiter"
%token INSERT           "insert"
%token INTO             "into"
%token VALUES           "values"
%token EOF 0            "eof"

// Precedence
%left		OR
%left		AND
%right		NOT
%nonassoc   BETWEEN        // give BETWEEN higher precedence than AND/OR
%nonassoc	LT GT EQ LE GE NEQ1 NEQ2
%left		PLUS MINUS
%left		ASTERISK SOLIDUS MOD
%left		POW
%right		UMINUS
// ---------------------------------------------------------------------------------------------------
%type <std::vector<std::unique_ptr<AST>>>         StmtMulti;
%type <std::unique_ptr<AST>>                      Stmt;
%type <factDB::schemac::IndexType>                opt_index_type;

%type <std::unique_ptr<AST>>                      copy_stmt;
%type <std::unique_ptr<AST>>                      select_no_parens;
%type <std::unique_ptr<AST>>                      simple_select;
%type <std::string>                               opt_delimiter;
%type <std::string>                               Sconst;

%type <std::vector<std::unique_ptr<AST>>> opt_target_list;
%type <std::vector<std::unique_ptr<AST>>> target_list;
%type <std::unique_ptr<AST>> target_el;
%type <std::vector<std::unique_ptr<AST>>> from_clause;
%type <std::vector<std::unique_ptr<AST>>> from_list;
%type <std::unique_ptr<AST>> table_ref;
%type <std::unique_ptr<AST>> where_clause;
%type <std::unique_ptr<AST>> relation_expr;
%type <std::unique_ptr<AST>> a_expr;
%type <std::unique_ptr<AST>> c_expr;
%type <std::unique_ptr<AST>> AexprConst;
%type <std::unique_ptr<AST>> identifier;
%type <std::unique_ptr<AST>> ColId;
%type <std::unique_ptr<AST>> opt_alias_clause;
%type <std::unique_ptr<AST>> alias_clause;


%type <factDB::schemac::Table>                    parse_table;
%type <factDB::schemac::IndexType>                parse_index_type;
%type <std::vector<factDB::schemac::Column>>      parse_primary_key_list;
%type <factDB::schemac::Column>                   parse_primary_key;
%type <std::vector<factDB::schemac::Column>>      opt_attributes_decl_list;
%type <std::vector<factDB::schemac::Column>>      attributes_decl_list;
%type <factDB::schemac::Column>                   attributes_decl;

%type <factDB::schemac::Type>                     some_type;
%type <std::string>                               name;
// ---------------------------------------------------------------------------------------------------
%%

%start StmtBlock;

StmtBlock:
    StmtMulti                               { sc.registerAST(std::move($1)); }
;

StmtMulti:
    StmtMulti SEMICOLON Stmt                { $1.push_back(std::move($3)); $$ = std::move($1); }
 |  Stmt                                    { $$.push_back(std::move($1)); }
;

Stmt:
   select_no_parens %prec UMINUS            { $$ = std::move($1); }
 | parse_table                              { $$ = std::make_unique<factDB::parser::CreateTable>(std::move($1)); }
 | copy_stmt                                { $$ = std::move($1); }
 | %empty                                   { $$ = nullptr; }
;

select_no_parens:
   simple_select { $$ = std::move($1); }
;

simple_select:
   SELECT opt_target_list from_clause where_clause { $$ = std::make_unique<Select>(std::move($2), std::move($3), std::move($4)); }

opt_target_list:
   target_list                              { $$ = std::move($1); }
 | %empty                                   { $$ = std::vector<std::unique_ptr<AST>>(); }
;

target_list:
   target_el                                { $$.push_back(std::move($1)); }
 | target_list COMMA target_el              { $1.push_back(std::move($3)); $$ = std::move($1); }
;

target_el:
   COUNT LRB ASTERISK RRB                   { $$ = std::make_unique<Target>(std::make_unique<CountStar>(), nullptr); }
 | a_expr AS ColId                          { $$ = std::make_unique<Target>(std::move($1), std::move($3)); }
 | a_expr ColId                             { $$ = std::make_unique<Target>(std::move($1), std::move($2)); }
 | a_expr                                   { $$ = std::make_unique<Target>(std::move($1), nullptr); }
 | ASTERISK                                 { $$ = std::make_unique<Target>(nullptr, nullptr); }
;

from_clause:
   FROM from_list                           { $$ = std::move($2); }
;

from_list:
   table_ref                                { $$.push_back(std::move($1)); }
 | from_list COMMA table_ref                { $1.push_back(std::move($3)); $$ = std::move($1); }
;

table_ref:
   relation_expr opt_alias_clause           { $$ = std::make_unique<TableRef>(TableRef::SubType::Relation, std::move($1), std::move($2)); }
;

relation_expr:
   identifier { $$ = std::make_unique<RelationExpr>(std::move($1)); }
;

ColId:
   identifier { $$ = make_unique<ColId>(nullptr, std::move($1)); }
 | identifier DOT identifier { $$ = make_unique<ColId>(std::move($1), std::move($3)); }
;

identifier:
    IDENT { $$ = make_unique<Identifier>($1, false); }
 |  QUOTED_IDENT { $$ = make_unique<Identifier>(std::string($1.begin() + 1, $1.begin() + $1.length() - 1), true); }

opt_alias_clause:
   alias_clause { $$ = std::move($1); }
 | %empty { $$ = nullptr; }
;

alias_clause:
   AS IDENT { $$ = std::make_unique<Alias>(std::move($2)); }
 | IDENT { $$ = std::make_unique<Alias>(std::move($1)); }
;

where_clause:
   WHERE a_expr { $$ = std::move($2); }
 | %empty { $$ = nullptr; }
;

a_expr:
   c_expr { $$ = std::move($1); }
 | PLUS a_expr %prec UMINUS { $$ = std::make_unique<UnaryExpression>(UnaryExpression::SubType::UPLUS, std::move($2)); }
 | MINUS a_expr %prec UMINUS { $$ = std::make_unique<UnaryExpression>(UnaryExpression::SubType::UMINUS, std::move($2)); }
 | a_expr PLUS a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::PLUS, std::move($1), std::move($3)); }
 | a_expr MINUS a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::MINUS, std::move($1), std::move($3)); }
 | a_expr ASTERISK a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::MUL, std::move($1), std::move($3)); }
 | a_expr SOLIDUS a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::DIV, std::move($1), std::move($3)); }
 | a_expr MOD a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::MOD, std::move($1), std::move($3)); }
 | a_expr POW a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::POW, std::move($1), std::move($3)); }
 | a_expr LT a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::LT, std::move($1), std::move($3)); }
 | a_expr GT a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::GT, std::move($1), std::move($3)); }
 | a_expr EQ a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::EQ, std::move($1), std::move($3)); }
 | a_expr LE a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::LE, std::move($1), std::move($3)); }
 | a_expr GE a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::GE, std::move($1), std::move($3)); }
 | a_expr NEQ1 a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::NEQ, std::move($1), std::move($3)); }
 | a_expr NEQ2 a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::NEQ, std::move($1), std::move($3)); }
 | a_expr AND a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::AND, std::move($1), std::move($3)); }
 | a_expr OR a_expr { $$ = std::make_unique<BinaryExpression>(BinaryExpression::SubType::OR, std::move($1), std::move($3)); }
 | a_expr BETWEEN c_expr AND c_expr { $$ = std::make_unique<BetweenExpression>(std::move($1), std::move($3), std::move($5)); }
 | NOT a_expr { $$ = std::make_unique<UnaryExpression>(UnaryExpression::SubType::NOT, std::move($2)); }
;

c_expr:
   ColId { $$ = std::move($1); }
 | AexprConst { $$ = std::move($1); }
 | LRB a_expr RRB { $$ = std::move($2); }
;

AexprConst:
   INTEGER_VALUE { $$ = std::make_unique<Literal>(Literal::SubType::INT, $1); }
 | NUMBER { $$ = std::make_unique<Literal>(Literal::SubType::NUMERIC, $1); }
 | Sconst { $$ = std::make_unique<Literal>(Literal::SubType::STRING, $1); }
 | TRUE { $$ = std::make_unique<Literal>(Literal::SubType::TRUE, ""); }
 | FALSE { $$ = std::make_unique<Literal>(Literal::SubType::FALSE, ""); }
;

copy_stmt:
    COPY name FROM Sconst opt_delimiter     { $$ = std::make_unique<factDB::parser::CopyTable>(std::move($2), std::move($4), std::move($5)); }
;

opt_delimiter:
    DELIMITER Sconst { $$ = std::move($2); }
 |  %empty { $$ = ""; }
;

Sconst:
    STRING_LITERAL { $$ = std::string($1.begin() + 1, $1.begin() + $1.length() - 1); }
;

opt_index_type:
    WITH LRB parse_index_type RRB                               { $$ = $3; }
 |  %empty                                                      { $$ = schemac::IndexType::None; }

parse_table: // table
    CREATE TABLE name LRB attributes_decl_list COMMA PRIMARY KEY LRB parse_primary_key_list RRB RRB opt_index_type                { $$.name = $3; $$.columns = $5; $$.primary_key = $10; $$.index_type = $13; }
 |  CREATE TABLE name LRB opt_attributes_decl_list RRB opt_index_type                                                             { $$.name = $3; $$.columns = $5; $$.index_type = $7; }

parse_index_type: // indextype
    name EQ UNORDERED_MAP                                       { $$ = schemac::IndexType::kSTLUnorderedMap; }
 |  name EQ BTREE_MAP                                           { $$ = schemac::IndexType::kSTXMap; }
 |  name EQ ORDERED_MAP                                         { $$ = schemac::IndexType::kSTLMap; }

parse_primary_key_list: // vec of columns
    parse_primary_key_list COMMA parse_primary_key              { $1.push_back($3); std::swap($$, $1); }
 |  parse_primary_key                                           { $$.push_back(std::move($1)); }

parse_primary_key: // column
    name                                                        { $$.name = $1; }

opt_attributes_decl_list:
    attributes_decl_list                                        { $$ = std::move($1); }
 | %empty                                                       { $$ = std::vector<factDB::schemac::Column>(); }

attributes_decl_list: // vec of columns
    attributes_decl_list COMMA attributes_decl                  { $1.push_back($3); std::swap($$, $1); }
 |  attributes_decl                                             { $$.push_back(std::move($1)); }

attributes_decl: // column
    name some_type NOT NULL                                     { $$.name = $1; $$.type = $2; }
 |  name some_type                                              { $$.name = $1; $$.type = $2; }

some_type: // Type
    INT                                                         { $$ = schemac::Type::Integer(); }
 |  INTEGER                                                     { $$ = schemac::Type::Integer(); }
 |  UINT64                                                      { $$ = schemac::Type::UInt64(); }
 |  CHAR LRB INTEGER_VALUE RRB                                  { $$ = schemac::Type::Char(std::stoul($3)); }
 |  TIMESTAMP                                                   { $$ = schemac::Type::Timestamp(); }
 |  DATE                                                        { $$ = schemac::Type::Date(); }
 |  VARCHAR LRB INTEGER_VALUE RRB                               { $$ = schemac::Type::Varchar(std::stoul($3)); }
 |  NUMERIC LRB INTEGER_VALUE COMMA INTEGER_VALUE RRB           { $$ = schemac::Type::Numeric(std::stoul($3), std::stoul($5)); }

name: // <std::string>
    IDENT                                                       { /*util::tolower($1);*/ $$ = std::move($1); }
 |  QUOTED_IDENT                                                { /*util::tolower($1);*/ $$ = std::string($1.begin() + 1, $1.end() - 1); }

%%
// ---------------------------------------------------------------------------------------------------
// Define error function
void factDB::parser::SchemaParser::error(const location_type& l, const std::string& m) {
    sc.Error(l.begin.line, l.begin.column, m);
}
// ---------------------------------------------------------------------------------------------------

