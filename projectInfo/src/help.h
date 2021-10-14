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
#include <chrono>

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

std::string trim(const std::string& s);
std::string ltrim(const std::string& s);
std::string rtrim(const std::string& s);
std::string replaceAllr(const std::string& subject,
		const std::string& regexFrom, const std::string& to);

VString splitr(const std::string& subject, const std::string& regex,
		bool includeSeparators = true);

std::size_t getBalanceBracketsPos(const std::string& s, SURROUND su);

std::string toHTML(const std::string& s,bool withKeywords=true);
std::string spanSurround(std::string s, std::string _class);
int addLines(const std::string& s, int lines);
int countLines(const std::string& s);

void proceed(int n,VContentInfo*);

template <typename TP>
std::time_t to_time_t(TP tp){
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
              + system_clock::now());
    return system_clock::to_time_t(sctp);
}

#endif /* HELP_H_ */
