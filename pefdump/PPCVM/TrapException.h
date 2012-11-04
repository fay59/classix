#ifndef TRAPEXCEPTION_H
#define TRAPEXCEPTION_H

#include <exception>
#include <string>

namespace PPCVM
{
	class TrapException : public std::exception
	{
		std::string trapName;
	public:
		TrapException(const std::string& trapName);
		
		virtual const char* what() const noexcept;
	};
}

#endif