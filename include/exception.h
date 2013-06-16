#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string.h>


class Exception
{
public:
	Exception(const char* location, const char* error)
	{
		m_location = strdup(location);
		m_errorText = strdup(error);
	}

	Exception(const Exception& rhs)
	{
		m_errorText = strdup(rhs.m_errorText);
		m_location = strdup(rhs.m_location);
	}

	virtual ~Exception()
	{
		free(m_location);
		free(m_errorText);
	}

	virtual const char* toString()
	{
		return("This feature not finished - use getLocation() and getError()");
	}
	virtual const char* getLocation()
	{
		return m_location;
	}
	virtual const char* getError()
	{
		return m_errorText;
	}
private:
	char* m_location;
	char* m_errorText;
};

class NullPointerException : public Exception
{
public:
	NullPointerException(const char* location, const char* error)
		: Exception( location, error )
	{ }
};

#endif
