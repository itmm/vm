find_package(GTest REQUIRED)

add_executable(Tests
	test-add-int.cpp
	test-bool.cpp
	test-ch-to-int.cpp
	test-div-int.cpp
	test-dup.cpp
	test-equals.cpp
	test-fetch.cpp
	test-less.cpp
	test-mod-int.cpp
	test-mult-int.cpp
	test-pull.cpp
	test-sub-int.cpp
	test-swap.cpp
	test-push-pull.cpp
	vm-tests.cpp
)

target_link_libraries(Tests PRIVATE GTest::gtest GTest::gtest_main libvm liberrmsgs)
