#include "err.h"

using namespace vm;

[[noreturn]] void vm::err(Err::Code code) { throw Err { code }; }
