#ifndef EXCEPTION_HH_
#define EXCEPTION_HH_

#include 					<string>
#include 					<exception>

class Exception : public std::exception
{
	protected:
		std::string 	message;
	public:
		Exception(const std::string & msg) throw();
		virtual const char *what(void) const throw();
		virtual ~Exception(void) throw();
};

class ParameterException : public Exception
{
	public:
		ParameterException(const std::string & msg) throw();
		virtual ~ParameterException(void) throw();
};

class InputNotFoundException : public Exception
{
	public:
		InputNotFoundException(const std::string & msg) throw();
		virtual ~InputNotFoundException(void) throw();
};

class FileReadException : public Exception
{
	public:
		FileReadException(const std::string & msg) throw();
		virtual ~FileReadException(void) throw();
};

#endif
