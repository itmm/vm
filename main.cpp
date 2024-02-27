#include "asm.h"
#include "err.h"
#include "errmsgs.h"

#include <iostream>

int main() {
	signed char ram[4096];
	signed char code[] {
		#if CONFIG_WITH_CHAR
			PUSH_CH('?'), vm::op_write_ch,
		#endif
		vm::op_break
	};
	try {
		vm::init(ram, ram + sizeof(ram), code, code + sizeof(code));
		for (;;) { vm::step(); }
	} catch (const vm::Err& err) {
		std::cerr << "FAILED: " << err_msgs[err.code] << "\n";
	}
	return 0;
}
