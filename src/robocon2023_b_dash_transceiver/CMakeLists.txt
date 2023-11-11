cmake_minimum_required(VERSION 3.5)

project(robocon2023_b_dash_transceiver)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sdl2_vendor REQUIRED)
find_package(sdl2_custom REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

add_library(joy SHARED src/joy.cpp)
target_include_directories(joy PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")
ament_target_dependencies(joy
  rclcpp
  rclcpp_components
  sensor_msgs)
target_link_libraries(joy
  SDL2::SDL2)

install(TARGETS joy EXPORT export_joy
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

rclcpp_components_register_node(joy
  PLUGIN "joy::Joy"
  EXECUTABLE joy_node)

add_executable(joy_enumerate_devices
  src/joy_enumerate_devices.cpp)
target_link_libraries(joy_enumerate_devices
  SDL2::SDL2)

install(TARGETS joy_enumerate_devices
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

install(DIRECTORY config launch
  DESTINATION share/${PROJECT_NAME})

ament_export_targets(export_joy)
ament_export_dependencies(
  "rclcpp"
  "sensor_msgs"
  "sdl2_vendor"
  "sdl2_custom")

add_executable(usb920_node src/usb920_node.cpp)
target_include_directories(usb920_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(usb920_node PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

ament_target_dependencies(usb920_node
 rclcpp
 std_msgs
 sensor_msgs
)

install(TARGETS usb920_node
  DESTINATION lib/${PROJECT_NAME})


  add_executable(program_checker_node src/program_checker.cpp)
  
  ament_target_dependencies(program_checker_node
   rclcpp
   std_msgs
   sensor_msgs
  )
  
  install(TARGETS program_checker_node
    DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
