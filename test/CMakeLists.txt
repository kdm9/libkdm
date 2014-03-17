#CFLAGS
include_directories(tinytest)
add_executable(test_kdm test.c tinytest/tinytest.c)
#target_link_libraries(test_kdm z)

add_custom_command(TARGET test_kdm COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)
add_test(NAME test_kdm COMMAND test_kdm)
