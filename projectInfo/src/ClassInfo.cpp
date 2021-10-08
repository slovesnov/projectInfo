/*
 * ClassInfo.cpp
 *
 *       Created on: 28.12.2020
 *           Author: aleksey slovesnov
 * Copyright(c/c++): 2020-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         Homepage: slovesnov.users.sourceforge.net
 */

#include <cassert>
#include "ClassInfo.h"

bool ClassInfo::check(const std::string& s, VString const& classes, int curly,
		std::string const& fileName, int lines) {
	/* get class inheritance ONLY IF next "{" because
	 * "class Frame;" is not declaration so match till the end
	 * "iclass ...." is not a class
	 */

	std::string B = "\\s*((public|protected|private)(\\s+virtual)?\\s+)?([\\w:<>]+)\\s*";
	std::regex r(
			"(^|[^\\w<])(enum[\\r\\n\\s]+class|class|struct|union)\\s+(\\w+)\\s*(:" + B + "(," + B
					+ ")*)?$");

	int i, j, k;
	std::smatch match;
	std::string q;
	VString v;
	PStringString c;

	if (!regex_search(s, match, r)) {
		return false;
	}

	file = fileName;
	line = countLines(s.substr(0, match.position(0)), lines);
	q = match.str(0);
	if (!q.empty() && q[0] == '\n') { //because check first symbol in regex not '<'
		line++;
		q = q.substr(1);
	}

	//clear name & base from previous recognition
	name = "";
	base.clear();

	/* c++ regex doesn't support negative lookahead, so split manually
	 * class V: public std::vector<int> shouldn't split double colon
	 *
	 * next code tested under gtk/glib
	 int i;
	 GRegex *regex;
	 gchar **a;
	 std::string s="123::456:789,00";
	 regex = g_regex_new("(?<!:):(?!:)|,", GRegexCompileFlags(0), GRegexMatchFlags(0), NULL);
	 a = g_regex_split(regex, s.c_str(), GRegexMatchFlags(0));
	 for (i = 0; a[i] != NULL; i++) {
	 g_print("[%s]", a[i]);
	 }

	 g_strfreev(a);
	 g_regex_unref (regex);
	 *
	 */
	j = int(q.length());
	for (k = i = 0; i < j; i++) {
		if (q[i] == ','
				|| (q[i] == ':' && (i == 0 || q[i - 1] != ':')
						&& (i == j - 1 || q[i + 1] != ':'))) {
			v.push_back(q.substr(k, i - k));
			v.push_back( { q[i] });
			k = i + 1;
		}
	}
	v.push_back(q.substr(k));

	for (i = 0; i < int(v.size()); i += 2) {
		auto a = splitr(trim(v[i]), "\\s+", false);
		if (i == 0) {
			assert(a.size() == 2 || a.size() == 3);
			type = a[0];
			if(a.size()==3){
				type+=' '+a[1];
			}

			//class inside class
			for (j = 0; j < curly; j++) {
				name += classes[j] + "::";
			}
			name += a[a.size()-1];
		}
		else {
			assert(a.size() >= 1 && a.size() <= 3);
			//{type of inheritance, base class name}
			c.second = a[a.size() - 1];
			c.first = "";
			for (j = 0; j < int(a.size() - 1); j++) {
				if (j) {
					c.first += ' ';
				}
				c.first += a[j];
			}
			base.push_back(c);
		}
	}

	functions = 0;
	return true;
}

std::string ClassInfo::info() {
	if (name.empty()) {
		return "global functions";
	}
	std::string s = type + " " + name;
	bool b = 1;
	for (auto& a : base) {
		if (b) {
			s += " :";
			b = 0;
		}
		else {
			s += ",";
		}

		if (!a.first.empty()) {
			s += " " + a.first;
		}
		s += " " + a.second;
	}
	return toHTML(s);
}

std::string ClassInfo::fileline() {
	return name.empty() ? "" : jsFileLine(file, line);
}

std::string ClassInfo::js() {
	VString v;
#define A(a) JS2(#a, a())
	A(info)
	JS2("c", functions)
	A(fileline)
	JS2("cd", childDirectly)
	JS2("ca", childAll)

	/* column for sorting of 'info' column
	 * always add 's' suffix at the end so got 'infos'
	 * should be added at the end of js object
	 */
	JS2("infos", name)
#undef A
	return surround(jc(v), CURLY);
}
