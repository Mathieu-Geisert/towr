cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(towr VERSION 1.3.2)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(ifopt 2.0.1 REQUIRED)


###########
## Build ##
###########
IF(NOT CMAKE_BUILD_TYPE MATCHES Release)
  message(STATUS "CMAKE_BUILD_TYPE not set to Release -> impacts performance")
endif()

# so dynamic library libtowr.so retains link to ifopt_core.so
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

# The motion-planning problem formulated through variables and constraints
add_library(${PROJECT_NAME} SHARED
  # sample formulation usage
  src/nlp_formulation.cc
  src/parameters.cc
  # variables
  src/nodes_variables.cc
  src/nodes_variables_all.cc
  src/nodes_variables_phase_based.cc
  src/phase_durations.cc
  # models
  src/robot_model.cc
  src/dynamic_model.cc
  src/single_rigid_body_dynamics.cc
  # constraints
  src/time_discretization_constraint.cc
  src/base_motion_constraint.cc
  src/terrain_constraint.cc
  src/swing_constraint.cc
  src/force_constraint.cc
  src/total_duration_constraint.cc
  src/dynamic_constraint.cc
  src/range_of_motion_constraint.cc
  src/spline_acc_constraint.cc
  src/linear_constraint.cc
  # costs
  src/node_cost.cc
  src/soft_constraint.cc
  # initialization
  src/gait_generator.cc
  src/monoped_gait_generator.cc
  src/biped_gait_generator.cc
  src/quadruped_gait_generator.cc
  # terrain
  src/height_map_examples.cc
  src/height_map.cc
  # helpers
  src/state.cc
  src/polynomial.cc
  src/spline.cc
  src/phase_spline.cc
  src/node_spline.cc
  src/nodes_observer.cc
  src/spline_holder.cc
  src/euler_converter.cc
  src/phase_durations_observer.cc
  include/towr/nlp_formulation.h
  include/towr/variables/phase_spline.h
  include/towr/variables/variable_names.h
  include/towr/variables/nodes_variables_phase_based.h
  include/towr/variables/nodes_variables.h
  include/towr/variables/nodes_variables_all.h
  include/towr/variables/spline.h
  include/towr/variables/state.h
  include/towr/variables/polynomial.h
  include/towr/variables/phase_durations.h
  include/towr/variables/nodes_observer.h
  include/towr/variables/cartesian_dimensions.h
  include/towr/variables/spline_holder.h
  include/towr/variables/phase_durations_observer.h
  include/towr/variables/euler_converter.h
  include/towr/variables/node_spline.h
  include/towr/models/single_rigid_body_dynamics.h
  include/towr/models/kinematic_model.h
  include/towr/models/dynamic_model.h
  include/towr/models/robot_model.h
  include/towr/models/endeffector_mappings.h
  include/towr/models/examples/hyq_model.h
  include/towr/models/examples/anymal_model.h
  include/towr/models/examples/monoped_model.h
  include/towr/models/examples/biped_model.h
  include/towr/costs/soft_constraint.h
  include/towr/costs/node_cost.h
  include/towr/constraints/swing_constraint.h
  include/towr/constraints/total_duration_constraint.h
  include/towr/constraints/spline_acc_constraint.h
  include/towr/constraints/terrain_constraint.h
  include/towr/constraints/force_constraint.h
  include/towr/constraints/dynamic_constraint.h
  include/towr/constraints/base_motion_constraint.h
  include/towr/constraints/range_of_motion_constraint.h
  include/towr/constraints/time_discretization_constraint.h
  include/towr/constraints/linear_constraint.h
  include/towr/parameters.h
  include/towr/initialization/biped_gait_generator.h
  include/towr/initialization/quadruped_gait_generator.h
  include/towr/initialization/gait_generator.h
  include/towr/initialization/monoped_gait_generator.h
  include/towr/terrain/height_map.h
  include/towr/terrain/examples/height_map_examples.h
)
target_link_libraries(${PROJECT_NAME} 
  PUBLIC 
    ifopt::ifopt_core
)


target_include_directories(${PROJECT_NAME} 
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)


#############
## Testing ##
#############
enable_testing()
# example problem solved with IPOPT
add_executable(${PROJECT_NAME}-example 
  test/hopper_example.cc
)
target_link_libraries(${PROJECT_NAME}-example  
  PRIVATE
    ${PROJECT_NAME} 
    ifopt::ifopt_ipopt
)
add_test(${PROJECT_NAME}-example ${PROJECT_NAME}-example)

# unit tests of costs/constraints
find_package(GTest QUIET)
if (TARGET GTest::GTest) # only build when modern targets exists

  add_executable(${PROJECT_NAME}-test
    test/dynamic_constraint_test.cc
    test/dynamic_model_test.cc
  )
  target_link_libraries(${PROJECT_NAME}-test
    PRIVATE
      ${PROJECT_NAME}
      GTest::GTest GTest::Main
  )
  add_test(${PROJECT_NAME}-test ${PROJECT_NAME}-test)
endif()


#############
## Install ##
#############
include(GNUInstallDirs) # for correct libraries locations across platforms
set(config_package_location "share/${PROJECT_NAME}/cmake") # for .cmake find-scripts installs
install(
  TARGETS ${PROJECT_NAME} ${PROJECT_NAME}-example 
  EXPORT ${PROJECT_NAME}-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/towr
)
# Copy header files
install(
  DIRECTORY include/towr/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/towr
)
# Auto generate and install the find scripts helpers
install(
  EXPORT ${PROJECT_NAME}-targets
  FILE ${PROJECT_NAME}-targets.cmake
  NAMESPACE towr::
  DESTINATION ${config_package_location}
)
# generate the file TowrConfigVersion.cmake
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/towr-config-version.cmake")
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${version_file}
  VERSION ${towr_VERSION}
  COMPATIBILITY SameMajorVersion
)
# install the two files in a place where cmake looks for them
install(
  FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/towr-config.cmake"   # self-written
    ${version_file}                                         # generated
  DESTINATION ${config_package_location}
)
# Install a Catkin 'package.xml' file. This is required by REP-136.
install(FILES package.xml DESTINATION share/${PROJECT_NAME})
