#include "vm.h"

#include <iostream>

char* err_msgs[] = {
	#define ERR(C) #C,
	#include "err.def"
};

int main() {
	signed char ram[4096];
	signed char code[] {
		vm::op_push_ch, 8, vm::op_ch_to_int,
		vm::op_ch_to_int, vm::op_push_ch, '0', vm::op_ch_to_int,
			vm::op_add_int, vm::op_int_to_ch, vm::op_write_ch,
		vm::op_ch_to_int, vm::op_push_ch, '0', vm::op_ch_to_int,
			vm::op_add_int, vm::op_int_to_ch, vm::op_write_ch,
		vm::op_ch_to_int, vm::op_push_ch, '0', vm::op_ch_to_int,
			vm::op_add_int, vm::op_int_to_ch, vm::op_write_ch,
		vm::op_ch_to_int, vm::op_push_ch, '0', vm::op_ch_to_int,
			vm::op_add_int, vm::op_int_to_ch, vm::op_write_ch,
		vm::op_break
	};
	try {
		vm::init(ram, ram + sizeof(ram), code, code + sizeof(code));
		for (;;) { vm::step(); }
	} catch (const vm::Error& err) {
		std::cerr << "FAILED: " << err_msgs[err.code] << "\n";
	}
	return 0;
}
