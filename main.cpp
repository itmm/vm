#include "asm.h"
#include "errmsgs.h"

#include <iostream>

int main() {
	signed char ram[4096];
	signed char code[] {
		PUSH_SMALL_INT(8),
		vm::op_ch_to_int, PUSH_CH('0'), vm::op_ch_to_int,
			vm::op_add_int, vm::op_int_to_ch, vm::op_write_ch,
		vm::op_ch_to_int, PUSH_CH('0'), vm::op_ch_to_int,
			vm::op_add_int, vm::op_int_to_ch, vm::op_write_ch,
		vm::op_ch_to_int, PUSH_CH('0'), vm::op_ch_to_int,
			vm::op_add_int, vm::op_int_to_ch, vm::op_write_ch,
		vm::op_ch_to_int, PUSH_CH('0'), vm::op_ch_to_int,
			vm::op_add_int, vm::op_int_to_ch, vm::op_write_ch,
		PUSH_CH('\n'), vm::op_write_ch, vm::op_break
	};
	try {
		vm::init(ram, ram + sizeof(ram), code, code + sizeof(code));
		for (;;) { vm::step(); }
	} catch (const vm::Error& err) {
		std::cerr << "FAILED: " << err_msgs[err.code] << "\n";
	}
	return 0;
}
