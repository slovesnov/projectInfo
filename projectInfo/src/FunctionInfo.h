/*
 * FunctionInfo.h
 *
 *       Created on: 27.12.2020
 *           Author: aleksey slovesnov
 * Copyright(c/c++): 2020-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         Homepage: slovesnov.users.sourceforge.net
 */

#ifndef FUNCTIONINFO_H_
#define FUNCTIONINFO_H_

#include "help.h"

struct FunctionInfo {
	std::string predicate;
	std::string className;
	std::string name;
	std::string parameters;
	std::string file;
	int line;
	VPStringSize comment;
	std::size_t recognizeFirst,pFirst;

	bool check(std::string const& s, std::size_t f, std::string const& e,
			VString const& classes, int curly, std::string const& fileName,
			int lines,VPStringSize const& vc);
	bool check1(std::string const& s, std::size_t f, std::string const& e,
			VString const& classes, int curly, std::string const& fileName,
			int lines);
	static bool isValidItem(std::string const& s);
	std::string js();

	bool getItem(std::string const& s, std::size_t& pos, std::string& r);
	//std::string string(); //for debug

	bool isTemplatePrecede(std::string const& s,int&pos);
};

DECLARE_VECTOR_TYPE(FunctionInfo)

#endif /* FUNCTIONINFO_H_ */
