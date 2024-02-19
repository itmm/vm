#include "ptr.h"

const signed char* vm::code_begin;
const signed char* vm::code_end;
signed char* vm::ram_begin;
signed char* vm::heap_end;
signed char* vm::stack_begin;
signed char* vm::ram_end;

vm::Code_Ptr vm::pc;
