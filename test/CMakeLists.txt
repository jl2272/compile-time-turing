cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()

  find_package(compile_time_turing CONFIG REQUIRED) # for intro, project_options, ...

  if(NOT TARGET compile_time_turing_options)
    message(FATAL_ERROR "Required config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

add_executable(tests turing_tests.cpp "include/bad.h")

target_link_libraries(
  tests
  PRIVATE compile_time_turing::compile_time_turing_warnings
          compile_time_turing::compile_time_turing_options)

target_include_directories(tests PUBLIC ${CMAKE_SOURCE_DIR}/include
                                                                  ${CMAKE_BINARY_DIR}/include)

                                                                  get_target_property(test_includes tests INCLUDE_DIRECTORIES)
message(STATUS "Includes for tests: ${test_includes}")


if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
    COMMAND_EXPAND_LISTS)
endif()