include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


include(CheckCXXSourceCompiles)


macro(compile_time_turing_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)

    message(STATUS "Sanity checking UndefinedBehaviorSanitizer, it should be supported on this platform")
    set(TEST_PROGRAM "int main() { return 0; }")

    # Check if UndefinedBehaviorSanitizer works at link time
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
    check_cxx_source_compiles("${TEST_PROGRAM}" HAS_UBSAN_LINK_SUPPORT)

    if(HAS_UBSAN_LINK_SUPPORT)
      message(STATUS "UndefinedBehaviorSanitizer is supported at both compile and link time.")
      set(SUPPORTS_UBSAN ON)
    else()
      message(WARNING "UndefinedBehaviorSanitizer is NOT supported at link time.")
      set(SUPPORTS_UBSAN OFF)
    endif()
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    if (NOT WIN32)
      message(STATUS "Sanity checking AddressSanitizer, it should be supported on this platform")
      set(TEST_PROGRAM "int main() { return 0; }")

      # Check if AddressSanitizer works at link time
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
      check_cxx_source_compiles("${TEST_PROGRAM}" HAS_ASAN_LINK_SUPPORT)

      if(HAS_ASAN_LINK_SUPPORT)
        message(STATUS "AddressSanitizer is supported at both compile and link time.")
        set(SUPPORTS_ASAN ON)
      else()
        message(WARNING "AddressSanitizer is NOT supported at link time.")
        set(SUPPORTS_ASAN OFF)
      endif()
    else()
      set(SUPPORTS_ASAN ON)
    endif()
  endif()
endmacro()

macro(compile_time_turing_setup_options)
  option(compile_time_turing_ENABLE_HARDENING "Enable hardening" ON)
  option(compile_time_turing_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    compile_time_turing_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    compile_time_turing_ENABLE_HARDENING
    OFF)

  compile_time_turing_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR compile_time_turing_PACKAGING_MAINTAINER_MODE)
    option(compile_time_turing_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(compile_time_turing_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(compile_time_turing_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(compile_time_turing_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(compile_time_turing_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(compile_time_turing_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(compile_time_turing_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(compile_time_turing_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(compile_time_turing_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(compile_time_turing_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(compile_time_turing_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(compile_time_turing_ENABLE_PCH "Enable precompiled headers" OFF)
    option(compile_time_turing_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(compile_time_turing_ENABLE_IPO "Enable IPO/LTO" ON)
    option(compile_time_turing_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(compile_time_turing_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(compile_time_turing_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(compile_time_turing_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(compile_time_turing_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(compile_time_turing_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(compile_time_turing_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(compile_time_turing_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(compile_time_turing_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(compile_time_turing_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(compile_time_turing_ENABLE_PCH "Enable precompiled headers" OFF)
    option(compile_time_turing_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      compile_time_turing_ENABLE_IPO
      compile_time_turing_WARNINGS_AS_ERRORS
      compile_time_turing_ENABLE_USER_LINKER
      compile_time_turing_ENABLE_SANITIZER_ADDRESS
      compile_time_turing_ENABLE_SANITIZER_LEAK
      compile_time_turing_ENABLE_SANITIZER_UNDEFINED
      compile_time_turing_ENABLE_SANITIZER_THREAD
      compile_time_turing_ENABLE_SANITIZER_MEMORY
      compile_time_turing_ENABLE_UNITY_BUILD
      compile_time_turing_ENABLE_CLANG_TIDY
      compile_time_turing_ENABLE_CPPCHECK
      compile_time_turing_ENABLE_COVERAGE
      compile_time_turing_ENABLE_PCH
      compile_time_turing_ENABLE_CACHE)
  endif()

  compile_time_turing_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (compile_time_turing_ENABLE_SANITIZER_ADDRESS OR compile_time_turing_ENABLE_SANITIZER_THREAD OR compile_time_turing_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(compile_time_turing_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(compile_time_turing_global_options)
  if(compile_time_turing_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    compile_time_turing_enable_ipo()
  endif()

  compile_time_turing_supports_sanitizers()

  if(compile_time_turing_ENABLE_HARDENING AND compile_time_turing_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR compile_time_turing_ENABLE_SANITIZER_UNDEFINED
       OR compile_time_turing_ENABLE_SANITIZER_ADDRESS
       OR compile_time_turing_ENABLE_SANITIZER_THREAD
       OR compile_time_turing_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${compile_time_turing_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${compile_time_turing_ENABLE_SANITIZER_UNDEFINED}")
    compile_time_turing_enable_hardening(compile_time_turing_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
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

  include(cmake/Sanitizers.cmake)
  compile_time_turing_enable_sanitizers(
    compile_time_turing_options
    ${compile_time_turing_ENABLE_SANITIZER_ADDRESS}
    ${compile_time_turing_ENABLE_SANITIZER_LEAK}
    ${compile_time_turing_ENABLE_SANITIZER_UNDEFINED}
    ${compile_time_turing_ENABLE_SANITIZER_THREAD}
    ${compile_time_turing_ENABLE_SANITIZER_MEMORY})

  set_target_properties(compile_time_turing_options PROPERTIES UNITY_BUILD ${compile_time_turing_ENABLE_UNITY_BUILD})

  if(compile_time_turing_ENABLE_PCH)
    target_precompile_headers(
      compile_time_turing_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(compile_time_turing_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    compile_time_turing_enable_cache()
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

  if(compile_time_turing_ENABLE_HARDENING AND NOT compile_time_turing_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR compile_time_turing_ENABLE_SANITIZER_UNDEFINED
       OR compile_time_turing_ENABLE_SANITIZER_ADDRESS
       OR compile_time_turing_ENABLE_SANITIZER_THREAD
       OR compile_time_turing_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    compile_time_turing_enable_hardening(compile_time_turing_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
