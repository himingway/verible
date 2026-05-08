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

// This file contains a set of inline tree building functions
// similar to concrete_syntax_tree.h's MakeNode and MakeTaggedNode
//
// These functions enforce that nodes are made with consistent number of
// children, structure, and tags.
//
// On use of std::forward():
// Each function is templated over every parameter in order to provide
// perfect argument forwarding using std::forward. This allows these functions
// to take left hand values and right hand values as parameters without
// having to wrap every argument in std::move().
// While the implementation of this code is pretty gross looking, it provides
// a very clean and concise interface.
//
// Other potential implementations could use template metaprogramming
// to restrict the number of variadic parameters or use macro's to get
// perfect argument forwarding without large amounts of templating. These
// were deemed worse options overall.

#ifndef VERIBLE_VERILOG_CST_VERILOG_TREEBUILDER_UTILS_H_
#define VERIBLE_VERILOG_CST_VERILOG_TREEBUILDER_UTILS_H_

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "verible/common/text/concrete-syntax-tree.h"
#include "verible/verilog/CST/verilog-nonterminals.h"

namespace verilog {

// Set of utility functions for embedding a statement into a certain context.
std::string EmbedInModule(std::string_view text);
std::string EmbedInClass(std::string_view text);
std::string EmbedInFunction(std::string_view text);
std::string EmbedInClassMethod(std::string_view text);

// Checks that symbol is a leaf and its text matches expected.
// Uses gunit's CHECK to raise error.
void ExpectString(const verible::SymbolPtr &symbol, std::string_view expected);
void ExpectString(const verible::Symbol *symbol, std::string_view expected);
void ExpectString(std::nullptr_t, std::string_view expected);

// Dispatches ExpectString for SymbolPtr-like or nullptr arguments.
template <typename T>
void ExpectSymbolString(T &&symbol, std::string_view expected) {
  if constexpr (std::is_same_v<std::decay_t<T>, std::nullptr_t>) {
    ExpectString(symbol, expected);
  } else {
    ExpectString(symbol.get(), expected);
  }
}

template <typename T1, typename T2, typename T3>
verible::SymbolPtr MakeParenGroup(T1 &&left_paren, T2 &&contents,
                                  T3 &&right_paren) {
  ExpectSymbolString(left_paren, "(");
  if (contents != nullptr) {
    if constexpr (std::is_same_v<std::decay_t<T3>, std::nullptr_t>) {
      // nullptr right_paren with non-null contents is invalid (not error-recovery).
      ExpectString(right_paren, ")");
    } else {
      if (right_paren != nullptr) ExpectSymbolString(right_paren, ")");
    }
  }  // else right_paren might be dropped due to error-recovery
  return verible::MakeTaggedNode(
      NodeEnum::kParenGroup, std::forward<T1>(left_paren),
      std::forward<T2>(contents), std::forward<T3>(right_paren));
}

template <typename T1, typename T2, typename T3>
verible::SymbolPtr MakeBracketGroup(T1 &&left_brace, T2 &&contents,
                                    T3 &&right_brace) {
  ExpectSymbolString(left_brace, "[");
  ExpectSymbolString(right_brace, "]");
  return verible::MakeTaggedNode(
      NodeEnum::kBracketGroup, std::forward<T1>(left_brace),
      std::forward<T2>(contents), std::forward<T3>(right_brace));
}

template <typename T1, typename T2, typename T3>
verible::SymbolPtr MakeBraceGroup(T1 &&left_brace, T2 &&contents,
                                  T3 &&right_brace) {
  ExpectSymbolString(left_brace, "{");
  ExpectSymbolString(right_brace, "}");
  return verible::MakeTaggedNode(
      NodeEnum::kBraceGroup, std::forward<T1>(left_brace),
      std::forward<T2>(contents), std::forward<T3>(right_brace));
}

}  // namespace verilog

#endif  // VERIBLE_VERILOG_CST_VERILOG_TREEBUILDER_UTILS_H_
