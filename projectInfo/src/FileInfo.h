/*
 * FileInfo.h
 *
 *       Created on: 28.12.2020
 *           Author: aleksey slovesnov
 * Copyright(c/c++): 2020-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         Homepage: slovesnov.users.sourceforge.net
 */

#ifndef FILEINFO_H_
#define FILEINFO_H_

#include "help.h"

struct FileInfo {
	std::string name;
	int size;
	int lines;
	std::time_t lastModification;
	std::string js();
};
DECLARE_VECTOR_TYPE(FileInfo)

#endif /* FILEINFO_H_ */
