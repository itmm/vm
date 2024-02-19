#include "err.h"

using namespace vm;

[[noreturn]] void vm::err(Error::Code code) { throw Error { code }; }
