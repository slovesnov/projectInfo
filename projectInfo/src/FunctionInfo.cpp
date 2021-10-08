/*
 * FunctionInfo.cpp
 *
 *       Created on: 27.12.2020
 *           Author: aleksey slovesnov
 * Copyright(c/c++): 2020-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         Homepage: slovesnov.users.sourceforge.net
 */

#include "FunctionInfo.h"
#include "help.h"

const std::string DEFINE = "#define";
const std::string TEMPLATE = "template";
const std::string CONST = "const";

const std::string VALID[] = {
		CONST,
		"inline",
		"static",
		"virtual",
		"typename",
		"*",
		"&",
		"*&" };

bool FunctionInfo::check(std::string const& s, std::size_t f,
		std::string const& e, VString const& classes, int curly,
		std::string const& fileName, int lines) {

//	printl("#"+s+"#");
//	printl("#"+e+"#");

	int i;
	VString v;
	std::string b, q;
	std::size_t p = 0, ff = f;

	getItem(s, ff, name);
	p = ff; //in case of constructor, store

	//ignore lambdas *name.rbegin()!=']'
	if (*name.rbegin() == ']') {
		return false;
	}

//	printinfo
	/* until two unrecognized items
	 */
	i = 0;
	while (1) {
//		printl(s)
		if (!getItem(s, ff, b)) {
			break;
		}

		if (b == "\n") {
			continue;
		}

		if (!isValidItem(b)) {
			if (++i == 2) {
				break;
			}
		}
//		printl(b)

		v.push_back(b);
		p = ff;
	}

	if (v.size() == 1 && v[0] == DEFINE) {
		return false;
	}

	p++;
	assert(p < s.size());

	i = curly - 1;
	className = i >= 0 && i < int(classes.size()) ? classes[i] : "";
	line = countLines(s.substr(0, p), lines);
	file = fileName;

	//rfind because Bridge::FS::init(){
	const char S[] = "::";
	if ((ff = name.rfind(S)) != std::string::npos) {
		className = name.substr(0, ff);
		name = name.substr(ff + strlen(S));
	}

	parameters = s.substr(f) + rtrim(e);
	if (name == className || name == "~" + className) {
		predicate = "";
		return true;
	}

	predicate = joinVInverse(v);


	/*	inline GtkWidget *getWidget() const {
	 * name="*getWidget"
	 */
	assert(!name.empty());
	if (strchr("*&", name[0])) {
		predicate += name[0];
		assert(name.length() > 1);
		name = name.substr(1);
	}

	//		const int*begin() const {
	const std::string A[] = { "*", "&" };
	if (name.find("operator") == std::string::npos) {
		for (auto&q : A) {
			auto f = name.find(q);
			if (f != std::string::npos) {
				b = name.substr(f + 1);
				predicate += " " + name.substr(0, f + 1);
				name = b;
			}
		}
	}

	if (!strchr("_~", name[0]) && !isalpha(name[0])) {
		printl("ERROR invalid name", name)
	}

	return true;
}

std::string FunctionInfo::js() {
	VString v;
	std::string s;
	if (!className.empty()) {
		s = className + "::";
	}

	JS2("p",
			toHTML(predicate) + " " + spanSurround(s + name, "b")
					+ toHTML(parameters))
	JS2("fileline", jsFileLine(file, line))

	/* column for sorting of 'p' column
	 * always add 's' suffix at the end so got 'ps'
	 * should be added at the end of js object
	 */
	JS2("ps", name)

	return surround(jc(v), CURLY);

}

std::string FunctionInfo::string() {
	VString v;
	std::string s;
	if (!className.empty()) {
		s = className + "::";
	}

	return predicate + " " + s + name + parameters + " " + file
			+ std::to_string(line);
}

bool FunctionInfo::isValidItem(std::string const& s) {
	return ONE_OF(s,VALID) || startsWith(s, TEMPLATE) || s == CONST + "*"
			|| s == CONST + "&";
}

bool FunctionInfo::getItem(std::string const& s, std::size_t& pos,
		std::string& r) {
	//use signed int for cycle for normal check f>=0, f1>=0
	int f = pos, f1;
	std::string q;

	for (f--; f >= 0 && isspace(s[f]); f--)
		;
	if (f <= 0 || s[f] == ';') {
		return false;
	}

	if(isTemplatePrecede(s,f)){
		q = s.substr(0, f);
		f1 = getBalanceBracketsPos(q, ANGLE);
		q = q.substr(f1 + 1, f - f1) + ">";
		f = f1;
	}
	//use for operators +-*/!
	for (f1 = f - 1; f1 >= 0; f1--) {
		if (isalnum(s[f1]) || strchr(":_#~+-*/=!", s[f1])) {
			continue;
		}

		if(isTemplatePrecede(s,f1)){
			f1 = getBalanceBracketsPos(s.substr(0, f1), ANGLE);
			continue;
		}

		break;
	}

	pos = f1;
	r = s.substr(f1 + 1, f - f1) + q;
	return true;
}

bool FunctionInfo::isTemplatePrecede(std::string const& s,int&pos){
	//use signed int for cycle for normal check f>=0
	int f;
	for(f=pos ; f>=0 && isspace(s[f]);f--);
	if(s[f]=='>'){
		pos=f;
		return true;
	}
	else{
		return false;
	}

}
