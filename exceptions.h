#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <exception>

/*!
 * \brief An exception that is thrown if change cannot be calculated
 */
class change_error : public std::exception
{
	inline const char* what() const throw()
	{
		return "change cannot be calculated";
	}
};

/*!
 * \brief An exception that is thrown if method is called with invalid value
 */
class value_error : public std::exception
{
	inline const char* what() const throw()
	{
		return "invalid value given";
	}
};

#endif
