#include "asm.h"
#include "err.h"
#include "errmsgs.h"

#include <iostream>

int main() {
	signed char ram[4096];
	signed char code[] {
		PUSH_CH('?'), vm::op_write_ch, vm::op_break
	};
	try {
		vm::init(ram, ram + sizeof(ram), code, code + sizeof(code));
		for (;;) { vm::step(); }
	} catch (const vm::Err& err) {
		std::cerr << "FAILED: " << err_msgs[err.code] << "\n";
	}
	return 0;
}
