#include 	"exception.hh"

Exception::Exception(const std::string & msg) throw()
	: message(msg)
{
}

Exception::~Exception(void) throw()
{
}

const char *Exception::what(void) const throw()
{
	return (this->message.data());
}

ParameterException::ParameterException(const std::string & msg) throw()
	: Exception(msg)
{
}

ParameterException::~ParameterException(void) throw()
{
}
