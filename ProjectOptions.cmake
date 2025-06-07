include(cmake/SystemLink.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


include(CheckCXXSourceCompiles)

macro(compile_time_turing_setup_options)
  option(compile_time_turing_ENABLE_COVERAGE "Enable coverage reporting" OFF)

  if(NOT PROJECT_IS_TOP_LEVEL OR compile_time_turing_PACKAGING_MAINTAINER_MODE)
    option(compile_time_turing_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(compile_time_turing_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(compile_time_turing_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(compile_time_turing_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(compile_time_turing_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(compile_time_turing_ENABLE_PCH "Enable precompiled headers" OFF)
    option(compile_time_turing_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(compile_time_turing_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(compile_time_turing_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(compile_time_turing_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(compile_time_turing_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(compile_time_turing_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(compile_time_turing_ENABLE_PCH "Enable precompiled headers" OFF)
    option(compile_time_turing_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      compile_time_turing_WARNINGS_AS_ERRORS
      compile_time_turing_ENABLE_USER_LINKER
      compile_time_turing_ENABLE_UNITY_BUILD
      compile_time_turing_ENABLE_CLANG_TIDY
      compile_time_turing_ENABLE_CPPCHECK
      compile_time_turing_ENABLE_COVERAGE
      compile_time_turing_ENABLE_PCH
      compile_time_turing_ENABLE_CACHE)
  endif()

endmacro()

macro(compile_time_turing_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(compile_time_turing_warnings INTERFACE)
  add_library(compile_time_turing_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  compile_time_turing_set_project_warnings(
    compile_time_turing_warnings
    ${compile_time_turing_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(compile_time_turing_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    compile_time_turing_configure_linker(compile_time_turing_options)
  endif()

  set_target_properties(compile_time_turing_options PROPERTIES UNITY_BUILD ${compile_time_turing_ENABLE_UNITY_BUILD})

  if(compile_time_turing_ENABLE_PCH)
    target_precompile_headers(
      compile_time_turing_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(compile_time_turing_ENABLE_CLANG_TIDY)
    compile_time_turing_enable_clang_tidy(compile_time_turing_options ${compile_time_turing_WARNINGS_AS_ERRORS})
  endif()

  if(compile_time_turing_ENABLE_CPPCHECK)
    compile_time_turing_enable_cppcheck(${compile_time_turing_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(compile_time_turing_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    compile_time_turing_enable_coverage(compile_time_turing_options)
  endif()

  if(compile_time_turing_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(compile_time_turing_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

endmacro()
