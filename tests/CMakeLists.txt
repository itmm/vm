find_package(GTest REQUIRED)

add_executable(Tests
	test-add.cpp
	test-ch-to-int.cpp
	test-push-pull.cpp
	vm-tests.cpp
)

target_link_libraries(Tests PRIVATE GTest::gtest GTest::gtest_main libvm)
