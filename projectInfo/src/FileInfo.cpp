/*
 * FileInfo.cpp
 *
 *       Created on: 28.12.2020
 *           Author: aleksey slovesnov
 * Copyright(c/c++): 2020-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         Homepage: slovesnov.users.sourceforge.net
 */

#include "FileInfo.h"

std::string FileInfo::js() {
	VString v;
	char b[80];
	//Note without numbers
	jsPath(name);
	//Note js parser supports month/day/year
	strftime(b, 80, "%m/%d/%Y %H:%M:%S", std::localtime(&lastModification));

#define A(a) JS2(#a, a);
	A(name)
	A(size)
	A(lines)
#undef A
	v.push_back(js2("time", b));

	return surround(jc(v), CURLY);

}

