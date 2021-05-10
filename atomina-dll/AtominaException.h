#pragma once
#include "api.h"

namespace ATMA {


	/*
	* Abstract Exception class, provides basic constructor and message storage
	* No default constructor is provided
	*/
	class ATMA_API AtominaException{

	protected:
		const char* msg_;
	public:

		AtominaException(const char* _Message) noexcept;

		virtual const char* what() = 0;

	};

	/*
	* Exception to be thrown when a function encounters a parameter with invalid values
	*/
	class ATMA_API InvalidParameterException : public AtominaException {

	public:
		InvalidParameterException(const char* _Message) noexcept;

		const char* what();
	};

}

