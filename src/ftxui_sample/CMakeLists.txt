add_executable(intro main.cpp)

target_link_libraries(
  intro
  PRIVATE compile_time_turing::compile_time_turing_options
          compile_time_turing::compile_time_turing_warnings)

target_link_system_libraries(
  intro
  PRIVATE
          CLI11::CLI11
          fmt::fmt
          spdlog::spdlog
          lefticus::tools
          ftxui::screen
          ftxui::dom
          ftxui::component)

target_include_directories(intro PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
