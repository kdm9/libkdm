#CFLAGS
include_directories(tinytest)
add_executable(test_kdm test.c tinytest/tinytest.c)
target_link_libraries(test_kdm z)

add_test(NAME test_kdm COMMAND test_kdm)
