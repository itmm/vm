#include "gtest/gtest.h"

#include "errmsgs.h"
#include "heap.h"
#include "vm-tests.h"

bool dump_heap { false };
bool dump_free { false };
bool dump_stack { false };

void expect_stack(
	const signed char* code_begin, int code_size,
	vm::Err::Code expected_error, int ram_size,
	const signed char* expected_begin, int expected_size
) {
	signed char ram[ram_size];
	try {
		vm::init(ram, ram + sizeof(ram), code_begin, code_begin + code_size);
		for (;;) { vm::step(); }
	} catch(const vm::Err& err) {
		if (err.code != expected_error) {
			std::cerr << "unexpected error: " << err_msgs[err.code] << "\n";
		}
		EXPECT_EQ(err.code, expected_error);
	}
	#if CONFIG_WITH_HEAP
		if (dump_heap) { vm::Heap::dump_heap(); }
	#endif
	if (dump_free) { std::cout << "free[" << vm::Stack_Ptr::begin - vm::stack_lower_limit() << "] { }\n"; }
	if (dump_stack) { vm::dump_stack(); }
	if (expected_begin) {
		EXPECT_EQ(vm::Stack_Ptr::begin + expected_size, ram + ram_size);
		for (int i { 0 }; i < expected_size; ++i) {
			EXPECT_EQ(vm::Stack_Ptr::begin[i], expected_begin[i]);
		}
	}
}
