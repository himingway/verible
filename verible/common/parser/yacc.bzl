# Copyright 2017-2020 The Verible Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""This module defines yacc/bison-related rules."""

def remove_const_from_glr_stack(name, src, out):
    """Remove const qualifier from GLR stack item access.

    Bison GLR casts stack access to 'yyGLRStackItem const *', making all
    $n values const. This prevents calling non-const methods like the
    ownership-releasing operator SymbolPtr() on GlrSymbolValue.
    This rule patches the generated code to use mutable stack access.

    Args:
      name: name of this label.
      src: a yacc/bison-generated .tab.cc source file.
      out: name of transformed source.
    """
    native.genrule(
        name = name,
        srcs = [src],
        outs = [out],
        cmd = "sed -e 's/yyGLRStackItem const \\*/yyGLRStackItem */g' < $< > $@",
    )

def record_recovered_syntax_errors(name, src, out):
    """Save syntax error tokens prior to error recovery.

    Args:
      name: name of this label.
      src: a yacc/bison-generated .tab.cc source file.
      out: name of transformed source.
    """
    native.genrule(
        name = name,
        srcs = [src],
        outs = [out],
        cmd = r"sed -e '/yynerrs += 1;/a\
          // Automatically patched by >>record_recovered_syntax_errors<< rule:\
          param->RecordSyntaxError(yylval);\
          // end of automatic patch\
          ' < $< > $@",
    )

# TODO(fangism): implement a .output (human-readable state-machine) reader.
