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

// GlrSymbolValue wraps a raw Symbol* for use as Bison GLR parser value type.
// Bison GLR stores values in a C-style union, so this type must have ALL
// trivial special member functions (default ctor, copy/move ctor/assign, dtor).
// This means: no default member initializer, no user-provided constructors,
// no user-provided destructors. Ownership is managed via operator SymbolPtr()
// which releases and clears the pointer.

#ifndef VERIBLE_COMMON_PARSER_GLR_SYMBOL_VALUE_H_
#define VERIBLE_COMMON_PARSER_GLR_SYMBOL_VALUE_H_

#include <memory>
#include <utility>

#include "verible/common/text/symbol-ptr.h"
#include "verible/common/text/symbol.h"

namespace verible {

class GlrSymbolValue {
 public:
  // All defaulted for union compatibility. ptr_ is uninitialized after
  // default construction; always assign before use (Bison guarantees this).
  GlrSymbolValue() = default;
  GlrSymbolValue(const GlrSymbolValue&) = default;
  GlrSymbolValue(GlrSymbolValue&&) = default;
  GlrSymbolValue& operator=(const GlrSymbolValue&) = default;
  GlrSymbolValue& operator=(GlrSymbolValue&&) = default;
  ~GlrSymbolValue() = default;

  // Construct/assign from nullptr.
  GlrSymbolValue(std::nullptr_t) : ptr_(nullptr) {}  // NOLINT
  GlrSymbolValue& operator=(std::nullptr_t) {
    ptr_ = nullptr;
    return *this;
  }

  // Construct/assign from SymbolPtr (takes ownership).
  GlrSymbolValue(SymbolPtr p) : ptr_(p.release()) {}  // NOLINT
  GlrSymbolValue& operator=(SymbolPtr p) {
    ptr_ = p.release();
    return *this;
  }

  // Conversion to SymbolPtr: releases ownership, clears pointer.
  operator SymbolPtr() {  // NOLINT
    Symbol* p = ptr_;
    ptr_ = nullptr;
    return SymbolPtr(p);
  }

  // Pointer-like access.
  Symbol& operator*() const { return *ptr_; }
  Symbol* operator->() const { return ptr_; }
  explicit operator bool() const { return ptr_ != nullptr; }

  Symbol* get() const { return ptr_; }
  void reset() { ptr_ = nullptr; }

  friend bool operator==(const GlrSymbolValue& v, std::nullptr_t) {
    return v.ptr_ == nullptr;
  }
  friend bool operator!=(const GlrSymbolValue& v, std::nullptr_t) {
    return v.ptr_ != nullptr;
  }
  friend bool operator==(std::nullptr_t, const GlrSymbolValue& v) {
    return v.ptr_ == nullptr;
  }
  friend bool operator!=(std::nullptr_t, const GlrSymbolValue& v) {
    return v.ptr_ != nullptr;
  }

 private:
  Symbol* ptr_;
};

}  // namespace verible

#endif  // VERIBLE_COMMON_PARSER_GLR_SYMBOL_VALUE_H_
