#include "vm.h"

#include <iostream>

char* err_msgs[] = {
	#define ERR(C) #C,
	#include "err.def"
};

int main() {
	char ram[4096];
	char code[] {
		vm::op_push_ch, 'v', vm::op_write_char,
		vm::op_push_ch, 'm', vm::op_write_char,
		vm::op_nop, vm::op_break
	};
	try {
		vm::init(ram, ram + sizeof(ram), code, code + sizeof(code));
		for (;;) { vm::step(); }
	} catch (const vm::Error& err) {
		std::cerr << "FAILED: " << err_msgs[err.code] << "\n";
	}
	return 0;
}
