find_package(GTest REQUIRED)

add_executable(Tests
	test-add.cpp
	test-bool.cpp
	test-call.cpp
	test-collect-garbage.cpp
	test-div.cpp
	test-dup.cpp
	test-equals.cpp
	test-fetch.cpp
	test-heap.cpp
	test-jmp.cpp
	test-less.cpp
	test-mod.cpp
	test-mult.cpp
	test-pull.cpp
	test-push-pull.cpp
	test-send-receive.cpp
	test-store.cpp
	test-sub.cpp
	test-swap.cpp
	test-to-ch.cpp
	test-to-int.cpp
	test-tree.cpp
	vm-tests.cpp
)

target_link_libraries(Tests PRIVATE GTest::gtest GTest::gtest_main libvm liberrmsgs)
