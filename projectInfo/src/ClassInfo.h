/*
 * ClassInfo.h
 *
 *       Created on: 28.12.2020
 *           Author: aleksey slovesnov
 * Copyright(c/c++): 2020-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         Homepage: slovesnov.users.sourceforge.net
 */

#ifndef CLASSINFO_H_
#define CLASSINFO_H_

#include "types.h"
#include "aslov.h"

class ClassInfo {
public:
	std::string type, name;
	VPStringString base; //{type of inheritance, base class name}
	std::string file;
	int line;
	int functions;
	int childDirectly, childAll;
	bool check(std::string const& s, VString const& classes, int curly,
			std::string const& fileName, int lines);
	std::string info();
	std::string fileline();
	std::string js();

	bool operator<(ClassInfo const& c)const;
};
DECLARE_VECTOR_TYPE(ClassInfo)

#endif /* CLASSINFO_H_ */
