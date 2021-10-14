/*
 * ContentInfo.h
 *
 *  Created on: 14.10.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef SRC_CONTENTINFO_H_
#define SRC_CONTENTINFO_H_

#include "FunctionInfo.h"
#include "ClassInfo.h"
using MSCI =std::map<std::string,ClassInfo>;

class ContentInfo {
public:
	VFunctionInfo m_fi;
	MSCI m_ci;

	ContentInfo();
	virtual ~ContentInfo();
};

#endif /* SRC_CONTENTINFO_H_ */
