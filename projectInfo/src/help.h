/*
 * help.h
 *
 *       Created on: 26.12.2020
 *           Author: aleksey slovesnov
 * Copyright(c/c++): 2020-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         Homepage: slovesnov.users.sourceforge.net
 */

#ifndef HELP_H_
#define HELP_H_

#include <regex>

#include "types.h"
#include "ContentInfo.h"

#define JS2(a,b) v.push_back(js2(a, b));

//should match with SU array in help.cpp
enum SURROUND {
	CURLY, SQUARE, QUOTE, ROUND, ANGLE
};


std::string joinVInverse(VString const&v);
std::string jc(VString const &v, const char separator=',');
std::string surround(std::string s, SURROUND a);
std::string js2(const char *n, std::string s, SURROUND a = QUOTE);
std::string js2(const char *n, int s);
std::string jsFileLine(const std::string& file, int line);
void jsPath(std::string& s);

std::string replaceAllr(const std::string& subject,
		const std::string& regexFrom, const std::string& to);

VString splitr(const std::string& subject, const std::string& regex,
		bool includeSeparators = true);

std::size_t getBalanceBracketsPos(const std::string& s, SURROUND su);

std::string toHTML(const std::string& s,bool withKeywords=true);
std::string spanSurround(std::string s, std::string _class);
int addLines(const std::string& s, int lines);
int countLines(const std::string& s);

void proceed(int n,VContentInfo* pv);

TStringBoolString getProjectOptions(const std::string& name);

void proceedDirectory(TStringBoolString const& t,bool proceedFunctions,bool deleteSkipFiles,bool ot);
#endif /* HELP_H_ */
