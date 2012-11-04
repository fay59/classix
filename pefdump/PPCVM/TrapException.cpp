#include "TrapException.h"

PPCVM::TrapException::TrapException(const std::string& trapName)
: trapName(trapName)
{}

const char* PPCVM::TrapException::what() const noexcept
{
	return trapName.c_str();
}