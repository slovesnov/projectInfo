/*
 * common.cpp
 *
 *       Created on: 26.12.2020
 *           Author: aleksey slovesnov
 * Copyright(c/c++): 2020-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         Homepage: slovesnov.users.sourceforge.net
 */

#include <cassert>

#include "help.h"

//https://en.cppreference.com/w/cpp/keyword
const std::string KEYWORDS[] = { "class", //class should goes first, because change on <span class=...
		"alignas",
		"alignof",
		"and",
		"and_eq",
		"asm",
		"atomic_cancel",
		"atomic_commit",
		"atomic_noexcept",
		"auto",
		"bitand",
		"bitor",
		"bool",
		"break",
		"case",
		"catch",
		"char",
		"char8_t",
		"char16_t",
		"char32_t",
		"compl",
		"concept",
		"const",
		"consteval",
		"constexpr",
		"constinit",
		"const_cast",
		"continue",
		"co_await",
		"co_return",
		"co_yield",
		"decltype",
		"default",
		"delete",
		"do",
		"double",
		"dynamic_cast",
		"else",
		"enum",
		"explicit",
		"export",
		"extern",
		"false",
		"float",
		"for",
		"friend",
		"goto",
		"if",
		"inline",
		"int",
		"long",
		"mutable",
		"namespace",
		"new",
		"noexcept",
		"not",
		"not_eq",
		"nullptr",
		"operator",
		"or",
		"or_eq",
		"private",
		"protected",
		"public",
		"reflexpr",
		"register",
		"reinterpret_cast",
		"requires",
		"return",
		"short",
		"signed",
		"sizeof",
		"static",
		"static_assert",
		"static_cast",
		"struct",
		"switch",
		"synchronized",
		"template",
		"this",
		"thread_local",
		"throw",
		"true",
		"try",
		"typedef",
		"typeid",
		"typename",
		"union",
		"unsigned",
		"using",
		"virtual",
		"void",
		"volatile",
		"wchar_t",
		"while",
		"xor",
		"xor_eq" };

const char*SU[] = { "{}", "[]", "''", "()", "<>" };

std::string surround(std::string s, SURROUND a) {
	return SU[a][0] + s + SU[a][1];
}

std::string js2(const char *n, std::string s, SURROUND a) {
	return std::string(n) + ":" + surround(s, a);
}

std::string js2(const char *n, int s) {
	return std::string(n) + ":" + std::to_string(s);
}

std::string jsFileLine(const std::string& file, int line) {
	std::string s = file;
	jsPath(s);
	return s + " " + std::to_string(line);
}

void jsPath(std::string& s) {
	std::replace(s.begin(), s.end(), '\\', '/');
}

std::string joinVInverse(VString const&v) {
	const char separator= ' ';
	std::string s;
	for(auto a=v.rbegin();a!=v.rend();a++){
		if(a!=v.rbegin()){
			s+=separator;
		}
		s+=*a;
	}
	return s;
}

std::string jc(VString const &v, const char separator/*=','*/){
	return joinV(v,separator);
}

std::string trim(const std::string& s) {
	std::string q = ltrim(s);
	return rtrim(q);
}

std::string ltrim(const std::string& s) {
	std::string::const_iterator it;
	for (it = s.begin(); it != s.end() && isspace(*it); it++)
		;
	return s.substr(it - s.begin());
}

std::string rtrim(const std::string& s) {
	std::string::const_reverse_iterator it;
	for (it = s.rbegin(); it != s.rend() && isspace(*it); it++)
		;
	return s.substr(0, s.length() - (it - s.rbegin()));
}

std::string replaceAllr(const std::string& subject,
		const std::string& regexFrom, const std::string& to) {
	VString v;
	std::regex re(regexFrom);
	std::string s = subject;
	std::string r;
	std::smatch match;
	while (regex_search(s, match, re)) {
		r += s.substr(0, match.position(0));
		r += to;
		s = match.suffix().str();
	}
	return r + s;
}

VString splitr(const std::string& subject, const std::string& regex,
		bool includeSeparators) {
	VString v;
	std::regex r(regex);
	std::string s = subject;

	std::smatch match;
	while (regex_search(s, match, r)) {
		v.push_back(s.substr(0, match.position(0)));
		if (includeSeparators) {
			v.push_back(match.str(0));
		}
		s = match.suffix().str();
	}
	v.push_back(s);
	return v;
}

std::size_t getBalanceBracketsPos(const std::string& s, SURROUND su) {
	std::string::const_reverse_iterator it;
	const char*p = SU[su];
	int i = 1;
	for (it = s.rbegin(); it != s.rend(); it++) {
		//ignore char constants for example '(' ')'
		if ((*it == p[0] || *it == p[1])
				&& (it != s.rbegin() || it != s.rend() || it[1] != '\''
						|| it[-1] != '\'')) {
			i += *it == p[1] ? 1 : -1;
			if (i == 0) {
				break;
			}
		}
	}

	if (it == s.rend()) {
		return std::string::npos;
	}
	else {
		return s.length() - (it - s.rbegin()) - 1;
	}
}

std::string toHTML(const std::string& s) {
	int i;
	std::string s1;

	const std::string FROM[] = { "&", "<", ">", "\n", "\t" };
	const std::string TO[] = { "&amp;", "&lt;", "&gt;", "<br>", "  " };
	for (i = 0; i < SIZEI(FROM); i++) {
		s1 = replaceAll(i == 0 ? s : s1, FROM[i], TO[i]);
	}

	//if two or move spaces replace it " &nbsp; &nbsp;..."
	auto v = splitr(s1, " {2,}");
	s1 = "";
	for (auto&a : v) {
		if (startsWith(a, "  ")) {
			for (i = 0; i < int(a.length()); i++) {
				s1 += i % 2 ? "&nbsp;" : " ";
			}
		}
		else {
			s1 += a;
		}
	}

	for (i = 0; i < SIZEI(KEYWORDS); i++) {
		s1 = replaceAllr(s1, "\\b" + KEYWORDS[i] + "\\b",
				spanSurround(KEYWORDS[i], "k"));
	}

	return s1;
}

std::string spanSurround(std::string s, std::string _class) {
	std::string c = "\""; //use double quotes because use in js objects
	return "<span class=" + c + _class + c + ">" + s + "</span>";
}

int addLines(const std::string& s, int lines) {
	return lines + countLines(s);
}

int countLines(const std::string& s) {
	return countOccurence(s, '\n');
}
