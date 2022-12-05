function(catchtest LINK_LIBRARIES)
  find_package(Catch2 3 REQUIRED)
  include(Catch)

  file(GLOB_RECURSE test_src CONFIGURE_DEPENDS "*_test.cpp")
  foreach(test_source_file ${test_src})
    get_filename_component(test_name ${test_source_file} NAME_WE)
    add_executable(${test_name} ${test_source_file})
    target_link_libraries(${test_name} ${LINK_LIBRARIES} Catch2::Catch2WithMain)
    catch_discover_tests(${test_name})
  endforeach()

endfunction()
