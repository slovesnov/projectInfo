/*
 * types.h
 *
 *  Created on: 14.10.2021
 *      Author: alexey slovesnov
 * copyright(c/c++): 2014-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         homepage: slovesnov.users.sourceforge.net
 */

#ifndef SRC_TYPES_H_
#define SRC_TYPES_H_

#include <string>
#include <vector>

#define DECLARE_VECTOR_TYPE(type) using V##type = std::vector<type>;

using PStringString = std::pair<std::string,std::string>;
using PStringChar = std::pair<std::string,char>;
using PStringSize = std::pair<std::string,std::size_t>;
using TStringBoolString = std::tuple<std::string,bool,std::string>;

DECLARE_VECTOR_TYPE(PStringString)
DECLARE_VECTOR_TYPE(PStringChar)
DECLARE_VECTOR_TYPE(PStringSize)



#endif /* SRC_TYPES_H_ */
