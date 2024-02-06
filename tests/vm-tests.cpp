#include "vm-tests.h"

#include "gtest/gtest.h"

void expect_stack(
	const signed char* code_begin, int code_size,
	vm::Error::Code expected_error, int ram_size,
	const signed char* expected_begin, int expected_size
) {
	signed char ram[ram_size];
	try {
		vm::init(ram, ram + sizeof(ram), code_begin, code_begin + code_size);
		for (;;) { vm::step(); }
	} catch(const vm::Error& err) { EXPECT_EQ(err.code, expected_error); }
	if (expected_begin) {
		EXPECT_EQ(vm::stack_begin() + expected_size, ram + sizeof(ram));
		for (int i { 0 }; i < expected_size; ++i) {
			EXPECT_EQ(vm::stack_begin()[i], expected_begin[i]);
		}
	}
}
