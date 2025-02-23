# Copyright (C) 2020  Matthew "strager" Glazar
# See end of file for extended copyright information.

cmake_minimum_required(VERSION 3.10)
include(QuickLintJSCompiler)
include(QuickLintJSTarget)

quick_lint_js_add_executable(
  quick-lint-js-test
  diag-collector.cpp
  diag-matcher.cpp
  failing-diag-reporter.cpp
  fake-configuration-filesystem.cpp
  filesystem-test.cpp
  parse-json.cpp
  parse-support.cpp
  quick-lint-js/array.h
  quick-lint-js/characters.h
  quick-lint-js/diag-collector.h
  quick-lint-js/diag-matcher.h
  quick-lint-js/dirty-set.h
  quick-lint-js/failing-diag-reporter.h
  quick-lint-js/file-matcher.h
  quick-lint-js/gtest.h
  quick-lint-js/mock-inotify.h
  quick-lint-js/mock-kqueue.h
  quick-lint-js/mock-win32.h
  quick-lint-js/parse-json.h
  quick-lint-js/parse-support.h
  quick-lint-js/spy-lsp-endpoint-remote.h
  quick-lint-js/spy-lsp-message-parser.h
  quick-lint-js/spy-visitor.h
  quick-lint-js/test-translation-table-generated.h
  quick-lint-js/tracking-memory-resource.h
  quick-lint-js/variable-analyzer-support.h
  spy-visitor.cpp
  test-array.cpp
  test-assert.cpp
  test-async-byte-queue.cpp
  test-bit.cpp
  test-buffering-diag-reporter.cpp
  test-buffering-visitor-stack.cpp
  test-buffering-visitor.cpp
  test-byte-buffer.cpp
  test-c-api.cpp
  test-c-string-list.cpp
  test-cli-location.cpp
  test-concat.cpp
  test-configuration-loader.cpp
  test-configuration.cpp
  test-crash.cpp
  test-debug-server.cpp
  test-diag-code-list.cpp
  test-diag-code.cpp
  test-diag-matcher.cpp
  test-diagnostic-formatter.cpp
  test-diagnostic.cpp
  test-dirty-set.cpp
  test-emacs-lisp-diag-reporter.cpp
  test-emacs-location.cpp
  test-event-loop.cpp
  test-file-canonical.cpp
  test-file-path.cpp
  test-file.cpp
  test-function-ref.cpp
  test-heap-function.cpp
  test-instance-tracker.cpp
  test-integer-decimal.cpp
  test-integer-hexadecimal.cpp
  test-json.cpp
  test-lex.cpp
  test-linked-bump-allocator.cpp
  test-linked-vector.cpp
  test-locale.cpp
  test-lsp-diag-reporter.cpp
  test-lsp-document-text.cpp
  test-lsp-json-rpc-message-parser.cpp
  test-lsp-location.cpp
  test-lsp-message-parser.cpp
  test-lsp-pipe-writer.cpp
  test-lsp-server.cpp
  test-lsp-uri.cpp
  test-lsp-workspace-configuration.cpp
  test-math-overflow.cpp
  test-math.cpp
  test-narrow-cast.cpp
  test-options.cpp
  test-output-stream.cpp
  test-padded-string.cpp
  test-parse-class.cpp
  test-parse-conditional-expression.cpp
  test-parse-expression-jsx.cpp
  test-parse-expression-statement.cpp
  test-parse-expression-typescript.cpp
  test-parse-expression.cpp
  test-parse-function.cpp
  test-parse-jsx.cpp
  test-parse-loop.cpp
  test-parse-module.cpp
  test-parse-statement.cpp
  test-parse-typescript-ambiguous.cpp
  test-parse-typescript-angle-type-assertion.cpp
  test-parse-typescript-class.cpp
  test-parse-typescript-declare-class.cpp
  test-parse-typescript-declare-function.cpp
  test-parse-typescript-declare-interface.cpp
  test-parse-typescript-declare-namespace.cpp
  test-parse-typescript-declare-type.cpp
  test-parse-typescript-declare-var.cpp
  test-parse-typescript-enum.cpp
  test-parse-typescript-function.cpp
  test-parse-typescript-generic-arrow.cpp
  test-parse-typescript-generic.cpp
  test-parse-typescript-interface.cpp
  test-parse-typescript-module.cpp
  test-parse-typescript-namespace.cpp
  test-parse-typescript-this-parameters.cpp
  test-parse-typescript-type.cpp
  test-parse-typescript-var.cpp
  test-parse-typescript.cpp
  test-parse-var.cpp
  test-parse-warning.cpp
  test-parse.cpp
  test-pipe-writer.cpp
  test-result.cpp
  test-simd.cpp
  test-sorted-search.cpp
  test-string-view.cpp
  test-temporary-directory.cpp
  test-text-diag-reporter.cpp
  test-thread.cpp
  test-trace-flusher.cpp
  test-trace-reader.cpp
  test-trace-writer.cpp
  test-tracking-memory-resource.cpp
  test-translation-table-generated.cpp
  test-translation.cpp
  test-try-catch-stack.cpp
  test-typescript-test.cpp
  test-utf-16.cpp
  test-utf-8.cpp
  test-variable-analyzer-arguments.cpp
  test-variable-analyzer-delete.cpp
  test-variable-analyzer-enum.cpp
  test-variable-analyzer-eval.cpp
  test-variable-analyzer-globals.cpp
  test-variable-analyzer-interface.cpp
  test-variable-analyzer-multiple-declarations.cpp
  test-variable-analyzer-namespace.cpp
  test-variable-analyzer-parse.cpp
  test-variable-analyzer-type.cpp
  test-variable-analyzer-typeof.cpp
  test-variable-analyzer-unused-shadow.cpp
  test-variable-analyzer.cpp
  test-vector-profiler.cpp
  test-vector.cpp
  test-vim-location.cpp
  test-vim-qflist-json-diag-reporter.cpp
  test-web-demo-location.cpp
  test-write-integer.cpp
  variable-analyzer-support.cpp
)
target_include_directories(quick-lint-js-test PRIVATE .)
target_link_libraries(
  quick-lint-js-test
  PRIVATE
  boost
  boost_json
  gmock
  gmock_main
  gtest
  quick-lint-js-boost-json
  quick-lint-js-lib
  quick-lint-js-lib-debug
  quick-lint-js-typescript-test-lib
)
if (QUICK_LINT_JS_FEATURE_DEBUG_SERVER)
  target_link_libraries(quick-lint-js-test PRIVATE mongoose)
endif ()
if (QUICK_LINT_JS_PRECOMPILE_HEADERS)
  target_precompile_headers(
    quick-lint-js-test
    PRIVATE
    <gmock/gmock.h>
    <gtest/gtest.h>
    <iostream>
    <simdjson.h>
  )
endif ()
quick_lint_js_enable_bigobj_if_supported(quick-lint-js-test)

# HACK(strager): LTO builds are very slow. Our tests don't
# *need* LTO optimizations. Disable LTO for our tests.
set_target_properties(
  quick-lint-js-test
  PROPERTIES
  INTERPROCEDURAL_OPTIMIZATION OFF
)

add_test(
  NAME quick-lint-js-test
  COMMAND quick-lint-js-test
)

# Disable MSVC warnings in googletest's headers.
# TODO(strager): Use MSVC's /external switches instead.
quick_lint_js_add_warning_options_if_supported(
  quick-lint-js-test
  PRIVATE
  /wd26495 # Always initialize a member variable
  /wd26812 # Prefer 'enum class' over 'enum'
)

# HACK(strager): UndefinedBehaviorSanitizer is buggy on Linux x86_64 with GCC
# 8.4.0. Disable some UBSAN checks to avoid false positives.
if (
  CMAKE_CXX_COMPILER_ID STREQUAL GNU
  AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0.0
)
  set_source_files_properties(
    test-lex.cpp
    PROPERTIES
    COMPILE_OPTIONS -fno-sanitize=null
  )
endif ()

find_package(ICU COMPONENTS uc)
if (ICU_FOUND)
  quick_lint_js_add_executable(
    quick-lint-js-test-lex-unicode
    test-lex-unicode.cpp
  )
  target_link_libraries(
    quick-lint-js-test-lex-unicode
    PRIVATE
    ICU::uc
    gmock
    gmock_main
    gtest
    quick-lint-js-lib
  )
  if (QUICK_LINT_JS_PRECOMPILE_HEADERS)
    target_precompile_headers(
      quick-lint-js-test-lex-unicode
      PRIVATE
      <gmock/gmock.h>
      <gtest/gtest.h>
    )
  endif ()

  add_test(
    NAME quick-lint-js-test-lex-unicode
    COMMAND quick-lint-js-test-lex-unicode
  )
endif ()

find_package(PythonInterp 3.7)
if (QUICK_LINT_JS_ENABLE_CLI AND PYTHONINTERP_FOUND)
  add_test(
    NAME quick-lint-js-test-cli
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test-cli.py
  )
  set_property(
    TEST quick-lint-js-test-cli
    APPEND PROPERTY ENVIRONMENT QUICK_LINT_JS_EXE=$<TARGET_FILE:quick-lint-js>
  )
endif ()

# quick-lint-js finds bugs in JavaScript programs.
# Copyright (C) 2020  Matthew "strager" Glazar
#
# This file is part of quick-lint-js.
#
# quick-lint-js is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# quick-lint-js is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with quick-lint-js.  If not, see <https://www.gnu.org/licenses/>.
