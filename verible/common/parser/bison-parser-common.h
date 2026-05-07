// Copyright 2017-2020 The Verible Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Common defines and tiny helper functions for Bison-based parsers.

#ifndef VERIBLE_COMMON_PARSER_BISON_PARSER_COMMON_H_
#define VERIBLE_COMMON_PARSER_BISON_PARSER_COMMON_H_

#include "verible/common/parser/glr-symbol-value.h"
#include "verible/common/parser/parser-param.h"
#include "verible/common/text/concrete-syntax-tree.h"

// Enable yyparse symbol stack tracing, as parser shifts/reduces.
// Same as %debug in .yc file.
#define YYDEBUG 1

// A macro to turn a symbol into a string
// These should be used judiciously in modern code. Prefer raw-string literal
// should you simply need to specify a large string.
#define AS_STRING(x) AS_STRING_INTERNAL(x)
#define AS_STRING_INTERNAL(x) #x

namespace verible {

// Calls to yylex() in generated code will be redirected here.
int LexAdapter(GlrSymbolValue *value, ParserParam *param);

// Calls to yyerror() in generated code will be redirected hee.
void ParseError(const ParserParam *param, const char *function_name,
                const char *message);

}  // namespace verible

// NOTE: This header is included at a point in the generated .tab.cc file
// where functions like yylex have been renamed to <PREFIX>lex with
// #define's, so only reference these reserved names you mean to refer
// to the substituted names.

// Lexer interface function.
// Called by Bison-generated parser to get a next token.
// TODO(fangism): control yylex prototype using YY_DECL, or embed param inside
//   FlexLexerAdapter class template.
inline int yylex(verible::GlrSymbolValue *value, verible::ParserParam *param) {
  return verible::LexAdapter(value, param);
}

// Error-reporting function.
// Called by Bison-generated parser when a recognition error occurs.
inline void yyerror(verible::ParserParam *param, const char *message) {
  // TODO(fangism): record and accumulate multiple errors with error recovery.
  // TODO(fangism): pass in ParserParam reference to save errors, and analyze
  //   stack state.  This may need to be refactored per-language.
  verible::ParseError(param, AS_STRING(yyparse), message);
}

#endif  // VERIBLE_COMMON_PARSER_BISON_PARSER_COMMON_H_
