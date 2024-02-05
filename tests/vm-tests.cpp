#include "vm-tests.h"

#include "gtest/gtest.h"

void expect_stack(
	const signed char* code_begin, int code_size,
	const signed char* expected_begin, int expected_size
) {
	signed char ram[1024];
	vm::init(ram, ram + sizeof(ram), code_begin, code_begin + code_size);
	try {
		for (;;) { vm::step(); }
	} catch(const vm::Error& err) {
		if (err.code != vm::Error::err_break) { throw; }
	}
	EXPECT_EQ(vm::stack_begin() + expected_size, ram + sizeof(ram));
	for (int i { 0 }; i < expected_size; ++i) {
		EXPECT_EQ(vm::stack_begin()[i], expected_begin[i]);
	}
}

void expect_error(
	const signed char* code_begin, int code_size,
	vm::Error::Code expected_error
) {
	signed char ram[1024];
	vm::init(ram, ram + sizeof(ram), code_begin, code_begin + code_size);
	try {
		for (;;) { vm::step(); }
	} catch(const vm::Error& err) {
		EXPECT_EQ(err.code, expected_error);
	}
}
