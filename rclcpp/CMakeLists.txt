cmake_minimum_required(VERSION 3.5)

project(rclcpp)

find_package(ament_cmake_ros REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rcl REQUIRED)
find_package(rcl_interfaces REQUIRED)
find_package(rcl_yaml_param_parser REQUIRED)
find_package(rmw REQUIRED)
find_package(rmw_implementation REQUIRED)
find_package(rosgraph_msgs REQUIRED)
find_package(rosidl_generator_cpp REQUIRED)
find_package(rosidl_typesupport_c REQUIRED)
find_package(rosidl_typesupport_cpp REQUIRED)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

set(${PROJECT_NAME}_SRCS
  src/rclcpp/any_executable.cpp
  src/rclcpp/callback_group.cpp
  src/rclcpp/client.cpp
  src/rclcpp/clock.cpp
  src/rclcpp/context.cpp
  src/rclcpp/contexts/default_context.cpp
  src/rclcpp/duration.cpp
  src/rclcpp/event.cpp
  src/rclcpp/exceptions.cpp
  src/rclcpp/executor.cpp
  src/rclcpp/executors.cpp
  src/rclcpp/expand_topic_or_service_name.cpp
  src/rclcpp/executors/multi_threaded_executor.cpp
  src/rclcpp/executors/single_threaded_executor.cpp
  src/rclcpp/graph_listener.cpp
  src/rclcpp/init_options.cpp
  src/rclcpp/intra_process_manager.cpp
  src/rclcpp/intra_process_manager_impl.cpp
  src/rclcpp/logger.cpp
  src/rclcpp/memory_strategies.cpp
  src/rclcpp/memory_strategy.cpp
  src/rclcpp/node.cpp
  src/rclcpp/node_options.cpp
  src/rclcpp/node_interfaces/node_base.cpp
  src/rclcpp/node_interfaces/node_clock.cpp
  src/rclcpp/node_interfaces/node_graph.cpp
  src/rclcpp/node_interfaces/node_logging.cpp
  src/rclcpp/node_interfaces/node_parameters.cpp
  src/rclcpp/node_interfaces/node_services.cpp
  src/rclcpp/node_interfaces/node_time_source.cpp
  src/rclcpp/node_interfaces/node_timers.cpp
  src/rclcpp/node_interfaces/node_topics.cpp
  src/rclcpp/node_interfaces/node_waitables.cpp
  src/rclcpp/parameter.cpp
  src/rclcpp/parameter_value.cpp
  src/rclcpp/parameter_client.cpp
  src/rclcpp/parameter_events_filter.cpp
  src/rclcpp/parameter_map.cpp
  src/rclcpp/parameter_service.cpp
  src/rclcpp/publisher_base.cpp
  src/rclcpp/qos.cpp
  src/rclcpp/qos_event.cpp
  src/rclcpp/service.cpp
  src/rclcpp/signal_handler.cpp
  src/rclcpp/subscription_base.cpp
  src/rclcpp/time.cpp
  src/rclcpp/time_source.cpp
  src/rclcpp/timer.cpp
  src/rclcpp/type_support.cpp
  src/rclcpp/utilities.cpp
  src/rclcpp/waitable.cpp
)

# "watch" template for changes
configure_file(
  "resource/logging.hpp.em"
  "logging.hpp.em.watch"
  COPYONLY
)
# generate header with logging macros
set(python_code
  "import em"
  "em.invoke(['-o', 'include/rclcpp/logging.hpp', '${CMAKE_CURRENT_SOURCE_DIR}/resource/logging.hpp.em'])")
string(REPLACE ";" "$<SEMICOLON>" python_code "${python_code}")
add_custom_command(OUTPUT include/rclcpp/logging.hpp
  COMMAND ${CMAKE_COMMAND} -E make_directory "include/rclcpp"
  COMMAND ${PYTHON_EXECUTABLE} ARGS -c "${python_code}"
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/logging.hpp.em.watch"
  COMMENT "Expanding logging.hpp.em"
  VERBATIM
)
list(APPEND ${PROJECT_NAME}_SRCS
  include/rclcpp/logging.hpp)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")

add_library(${PROJECT_NAME}
  ${${PROJECT_NAME}_SRCS})
# specific order: dependents before dependencies
ament_target_dependencies(${PROJECT_NAME}
  "rcl"
  "rcl_yaml_param_parser"
  "builtin_interfaces"
  "rosgraph_msgs"
  "rosidl_typesupport_cpp"
  "rosidl_generator_cpp")

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME}
  PRIVATE "RCLCPP_BUILDING_LIBRARY")

install(
  TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# specific order: dependents before dependencies
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

ament_export_dependencies(ament_cmake)
ament_export_dependencies(rcl)
ament_export_dependencies(builtin_interfaces)
ament_export_dependencies(rosgraph_msgs)
ament_export_dependencies(rosidl_typesupport_cpp)
ament_export_dependencies(rosidl_typesupport_c)
ament_export_dependencies(rosidl_generator_cpp)
ament_export_dependencies(rcl_yaml_param_parser)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(rmw_implementation_cmake REQUIRED)

  include(cmake/rclcpp_add_build_failure_test.cmake)

  ament_add_gtest(test_client test/test_client.cpp)
  if(TARGET test_client)
    ament_target_dependencies(test_client
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_client ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_create_timer test/test_create_timer.cpp)
  if(TARGET test_create_timer)
    ament_target_dependencies(test_create_timer
      "rcl_interfaces"
      "rmw"
      "rcl"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_create_timer ${PROJECT_NAME})
    target_include_directories(test_create_timer PRIVATE test/)
  endif()
  ament_add_gtest(test_expand_topic_or_service_name test/test_expand_topic_or_service_name.cpp)
  if(TARGET test_expand_topic_or_service_name)
    ament_target_dependencies(test_expand_topic_or_service_name
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_expand_topic_or_service_name ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_function_traits test/test_function_traits.cpp)
  if(TARGET test_function_traits)
    ament_target_dependencies(test_function_traits
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
  endif()
  ament_add_gtest(test_mapped_ring_buffer test/test_mapped_ring_buffer.cpp)
  if(TARGET test_mapped_ring_buffer)
    ament_target_dependencies(test_mapped_ring_buffer
      "rcl"
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
  endif()
  ament_add_gtest(test_intra_process_manager test/test_intra_process_manager.cpp)
  if(TARGET test_intra_process_manager)
    ament_target_dependencies(test_intra_process_manager
      "rcl"
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
  endif()
  ament_add_gtest(test_node test/test_node.cpp)
  if(TARGET test_node)
    ament_target_dependencies(test_node
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_node ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_node_interfaces__get_node_interfaces
    test/node_interfaces/test_get_node_interfaces.cpp)
  if(TARGET test_node_interfaces__get_node_interfaces)
    target_link_libraries(test_node_interfaces__get_node_interfaces ${PROJECT_NAME})
  endif()

  # TODO(wjwwood): reenable these build failure tests when I can get Jenkins to ignore their output
  # rclcpp_add_build_failure_test(build_failure__get_node_topics_interface_const_ref_rclcpp_node
  #   test/node_interfaces/test_does_not_compile/get_node_topics_interface_const_ref_rclcpp_node.cpp)
  # target_link_libraries(build_failure__get_node_topics_interface_const_ref_rclcpp_node
  #   ${PROJECT_NAME})

  # rclcpp_add_build_failure_test(build_failure__get_node_topics_interface_const_ptr_rclcpp_node
  #   test/node_interfaces/test_does_not_compile/get_node_topics_interface_const_ptr_rclcpp_node.cpp)
  # target_link_libraries(build_failure__get_node_topics_interface_const_ptr_rclcpp_node
  #   ${PROJECT_NAME})

  # rclcpp_add_build_failure_test(build_failure__get_node_topics_interface_const_ref_wrapped_node
  #   test/node_interfaces/test_does_not_compile/get_node_topics_interface_const_ref_wrapped_node.cpp)
  # target_link_libraries(build_failure__get_node_topics_interface_const_ref_rclcpp_node
  #   ${PROJECT_NAME})

  # rclcpp_add_build_failure_test(build_failure__get_node_topics_interface_const_ptr_wrapped_node
  #   test/node_interfaces/test_does_not_compile/get_node_topics_interface_const_ptr_wrapped_node.cpp)
  # target_link_libraries(build_failure__get_node_topics_interface_const_ptr_rclcpp_node
  #   ${PROJECT_NAME})

  ament_add_gtest(test_node_global_args test/test_node_global_args.cpp)
  if(TARGET test_node_global_args)
    ament_target_dependencies(test_node_global_args
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_node_global_args ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_parameter_client test/test_parameter_client.cpp)
  if(TARGET test_parameter_client)
    ament_target_dependencies(test_parameter_client
      "rcl_interfaces"
    )
    target_link_libraries(test_parameter_client ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_parameter_events_filter test/test_parameter_events_filter.cpp)
  if(TARGET test_parameter_events_filter)
    ament_target_dependencies(test_parameter_events_filter
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_parameter_events_filter ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_parameter test/test_parameter.cpp)
  if(TARGET test_parameter)
    ament_target_dependencies(test_parameter
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_parameter ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_parameter_map test/test_parameter_map.cpp)
  if(TARGET test_parameter_map)
    target_link_libraries(test_parameter_map ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_publisher test/test_publisher.cpp)
  if(TARGET test_publisher)
    ament_target_dependencies(test_publisher
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_publisher ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_publisher_subscription_count_api test/test_publisher_subscription_count_api.cpp)
  if(TARGET test_publisher_subscription_count_api)
    ament_target_dependencies(test_publisher_subscription_count_api
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_publisher_subscription_count_api ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_rate test/test_rate.cpp)
  if(TARGET test_rate)
    ament_target_dependencies(test_rate
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_rate
      ${PROJECT_NAME}
    )
  endif()
  ament_add_gtest(test_serialized_message_allocator test/test_serialized_message_allocator.cpp)
  if(TARGET test_serialized_message_allocator)
    ament_target_dependencies(test_serialized_message_allocator
      test_msgs
    )
    target_link_libraries(test_serialized_message_allocator
      ${PROJECT_NAME}
    )
  endif()
  ament_add_gtest(test_service test/test_service.cpp)
  if(TARGET test_service)
    ament_target_dependencies(test_service
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_service ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_subscription test/test_subscription.cpp)
  if(TARGET test_subscription)
    ament_target_dependencies(test_subscription
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_subscription ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_subscription_publisher_count_api test/test_subscription_publisher_count_api.cpp)
  if(TARGET test_subscription_publisher_count_api)
    ament_target_dependencies(test_subscription_publisher_count_api
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_subscription_publisher_count_api ${PROJECT_NAME})
  endif()
  find_package(test_msgs REQUIRED)
  ament_add_gtest(test_subscription_traits test/test_subscription_traits.cpp)
  if(TARGET test_subscription_traits)
    ament_target_dependencies(test_subscription_traits
      "rcl"
      "test_msgs"
    )
  endif()
  ament_add_gtest(test_find_weak_nodes test/test_find_weak_nodes.cpp)
  if(TARGET test_find_weak_nodes)
    ament_target_dependencies(test_find_weak_nodes
      "rcl"
    )
    target_link_libraries(test_find_weak_nodes ${PROJECT_NAME})
  endif()

  get_default_rmw_implementation(default_rmw)
  find_package(${default_rmw} REQUIRED)
  get_rmw_typesupport(typesupport_impls_cpp "${default_rmw}" LANGUAGE "cpp")
  get_rmw_typesupport(typesupport_impls_c "${default_rmw}" LANGUAGE "c")
  set(mock_msg_files
    "test/mock_msgs/srv/Mock.srv")
  rosidl_generate_interfaces(mock_msgs
    ${mock_msg_files}
    LIBRARY_NAME "rclcpp"
    SKIP_INSTALL)

  set(append_library_dirs "${CMAKE_CURRENT_BINARY_DIR}")
  if(WIN32)
    set(append_library_dirs "${append_library_dirs}/$<CONFIG>")
  endif()

  ament_add_gtest(test_externally_defined_services test/test_externally_defined_services.cpp
    APPEND_LIBRARY_DIRS "${append_library_dirs}")
  if(TARGET test_externally_defined_services)
    ament_target_dependencies(test_externally_defined_services
      "rcl"
    )
    target_link_libraries(test_externally_defined_services ${PROJECT_NAME})
    foreach(typesupport_impl_cpp ${typesupport_impls_cpp})
      rosidl_target_interfaces(test_externally_defined_services
        mock_msgs ${typesupport_impl_cpp})
    endforeach()
    foreach(typesupport_impl_c ${typesupport_impls_c})
      rosidl_target_interfaces(test_externally_defined_services
        mock_msgs ${typesupport_impl_c})
    endforeach()
  endif()

  ament_add_gtest(test_duration test/test_duration.cpp
    APPEND_LIBRARY_DIRS "${append_library_dirs}")
  if(TARGET test_duration)
    ament_target_dependencies(test_duration
      "rcl")
    target_link_libraries(test_duration ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_executor test/test_executor.cpp
    APPEND_LIBRARY_DIRS "${append_library_dirs}")
  if(TARGET test_executor)
    ament_target_dependencies(test_executor
      "rcl")
    target_link_libraries(test_executor ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_logger test/test_logger.cpp)
  target_link_libraries(test_logger ${PROJECT_NAME})

  ament_add_gmock(test_logging test/test_logging.cpp)
  target_link_libraries(test_logging ${PROJECT_NAME})

  ament_add_gtest(test_time test/test_time.cpp
    APPEND_LIBRARY_DIRS "${append_library_dirs}")
  if(TARGET test_time)
    ament_target_dependencies(test_time
      "rcl")
    target_link_libraries(test_time ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_timer test/test_timer.cpp
    APPEND_LIBRARY_DIRS "${append_library_dirs}")
  if(TARGET test_timer)
    ament_target_dependencies(test_timer
      "rcl")
    target_link_libraries(test_timer ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_time_source test/test_time_source.cpp
    APPEND_LIBRARY_DIRS "${append_library_dirs}")
  if(TARGET test_time_source)
    ament_target_dependencies(test_time_source
      "rcl")
    target_link_libraries(test_time_source ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_utilities test/test_utilities.cpp
    APPEND_LIBRARY_DIRS "${append_library_dirs}")
  if(TARGET test_utilities)
    ament_target_dependencies(test_utilities
      "rcl")
    target_link_libraries(test_utilities ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_init test/test_init.cpp
    APPEND_LIBRARY_DIRS "${append_library_dirs}")
  if(TARGET test_init)
    ament_target_dependencies(test_init
      "rcl")
    target_link_libraries(test_init ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_multi_threaded_executor test/executors/test_multi_threaded_executor.cpp
    APPEND_LIBRARY_DIRS "${append_library_dirs}")
  if(TARGET test_multi_threaded_executor)
    ament_target_dependencies(test_multi_threaded_executor
      "rcl")
    target_link_libraries(test_multi_threaded_executor ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_local_parameters test/test_local_parameters.cpp)
  if(TARGET test_local_parameters)
    ament_target_dependencies(test_local_parameters
      "rcl_interfaces"
      "rmw"
      "rosidl_generator_cpp"
      "rosidl_typesupport_cpp"
    )
    target_link_libraries(test_local_parameters ${PROJECT_NAME})
  endif()
endif()

ament_package()

install(
  DIRECTORY include/ ${CMAKE_CURRENT_BINARY_DIR}/include/
  DESTINATION include
)
