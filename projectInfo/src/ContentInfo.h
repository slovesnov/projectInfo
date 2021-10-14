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

class ContentInfo {

public:
	std::string file;
	std::string content;
	VFunctionInfo m_fi;
	MSCI m_ci;

	ContentInfo(std::string _file,std::string _content);

};

DECLARE_VECTOR_TYPE(ContentInfo)

#endif /* SRC_CONTENTINFO_H_ */
