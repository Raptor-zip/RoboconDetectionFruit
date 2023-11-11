cmake_minimum_required(VERSION 3.8)
project(robocon2023_b_dash_receiver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

add_executable(receiver src/receiver.cpp)
target_include_directories(receiver PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(receiver PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
ament_target_dependencies(receiver
 rclcpp
 std_msgs
 sensor_msgs
)
install(TARGETS receiver
  DESTINATION lib/${PROJECT_NAME})

add_executable(program_checker src/program_checker.cpp)
ament_target_dependencies(program_checker
 rclcpp
 std_msgs
 sensor_msgs
)
install(TARGETS program_checker
  DESTINATION lib/${PROJECT_NAME})


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
