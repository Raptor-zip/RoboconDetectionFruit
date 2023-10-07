cmake_minimum_required(VERSION 3.5)
project(robocon2023_b_dash)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

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


add_executable(SerialKIMD src/SerialKIMD.cpp)
ament_target_dependencies(SerialKIMD rclcpp std_msgs)
install(TARGETS SerialKIMD DESTINATION lib/${PROJECT_NAME})

add_executable(Xmove_Controller src/Xmove_Controller.cpp)
ament_target_dependencies(Xmove_Controller rclcpp std_msgs sensor_msgs)
install(TARGETS Xmove_Controller DESTINATION lib/${PROJECT_NAME})

add_executable(DriveMotion src/Drive_Motion.cpp)
ament_target_dependencies(DriveMotion rclcpp std_msgs sensor_msgs)
install(TARGETS DriveMotion DESTINATION lib/${PROJECT_NAME})

add_executable(Drive_Controller src/Drive_Controller.cpp)
ament_target_dependencies(Drive_Controller rclcpp std_msgs sensor_msgs)
install(TARGETS Drive_Controller DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
