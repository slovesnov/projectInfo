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

bool FunctionInfo::check(std::string const &s, const std::size_t f,
		std::string const &e, VString const &classes, int curly,
		std::string const &fileName, const int lines, VPStringSize const &vc) {
	if(!check1(s, f, e, classes, curly, fileName, lines)){
		return false;
	}

	//adjust lines
	comment.clear();
	for(auto const&b : vc){
		if (b.second > recognizeFirst) {
			line -= countLines(b.first);
		}
		if (b.second > pFirst) {
			comment.push_back( { b.first, b.second - pFirst });
		}
	}

	return true;
}

bool FunctionInfo::check1(std::string const &s, const std::size_t f,
		std::string const &e, VString const &classes, int curly,
		std::string const &fileName, const int lines) {

	int i;
	VString v;
	std::string b, q;
	std::size_t p = 0, ff = f,f1;

//	printzi('#',s.substr(0,f),"#",s.substr(f),"#",e,'#',lines)

	getItem(s, ff, name);
	f1=ff+1; //name.c_str()=s.c_str()+ff+1
	//i=1 "operator <(...)", i=0 "inline GtkWidget *getWidget()"
	i=1;
	for(auto c:name){
		if(isalnum(c)){
			i=0;
			break;
		}
	}
	if(i){
		getItem(s, ff, q);
//		printzi('[',q,']')
		name=q+" "+name;
	}

	p = ff; //in case of constructor, store

	//ignore lambdas *name.rbegin()!=']'
	if (*name.rbegin() == ']') {
		return false;
	}

	/* until two unrecognized items
	 */
	i = 0;
	while (getItem(s, ff, b)) {
		if (b == "\n") {
			continue;
		}

		if (!isValidItem(b)) {
			if (++i == 2) {
				break;
			}
		}

		v.push_back(b);
//		printl("[",b,"]")
		p = ff;
		recognizeFirst=ff+1;
	}
//	printvi(p);
	if (v.size() == 1 && v[0] == DEFINE) {
		return false;
	}

	p++;
	assert(p < s.size());

//	printl(joinV(v,'='))
//	printzi('=',name,'=',className,'=')

	//"#include <stdio.h>\n SearchResult::SearchResult()" in words projects
	if(!v.empty() && name[0]=='<' && v[v.size()-1].find("#include")!=std::string::npos){
//		printzi('[',s,']')
//		printzi('[',name,']')
		for(p=name.find('>')+1;p<name.length() && isspace(name[p]);p++);
		v.clear();
		name=name.substr(p);
		p+=f1;
	}
	else{
	}

	i = curly - 1;
	className = i >= 0 && i < int(classes.size()) ? classes[i] : "";
//	printvi(p);
//	printzi('[',s.substr(0, p),']',lines)
	line = addLines(s.substr(0, p), lines);
	file = fileName;

	//printzi('=',name,'=',className,'=')

	//rfind because Bridge::FS::init(){
	const char S[] = "::";
	if ((ff = name.rfind(S)) != std::string::npos) {
		className = name.substr(0, ff);
		name = name.substr(ff + strlen(S));
//		printzi('=',name,'=',className,'=')
	}

	pFirst=f;
	parameters = s.substr(f) + rtrim(e);
	bool constructor=name == className;
	bool destructor=name == "~" + className;
	if ( constructor || destructor) {
		predicate = "";
		recognizeFirst=pFirst-className.length();
		if(destructor){
			recognizeFirst--;
		}
		line = addLines(s.substr(0, recognizeFirst), lines);
//		printzi('[',s.substr(0, recognizeFirst),']',s.substr(recognizeFirst,4),lines)
//		printvi(predicate)
		return true;
	}

	predicate = joinVInverse(v);
//	printvi(predicate)

	//calculator exception.h	Exception(const char *message) :	std::exception() {
	if(predicate==":"){
		printzn(s.substr(0, recognizeFirst),"]")
		return check1( s, recognizeFirst-2,
				")", classes, curly,
				fileName, lines);
	}

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
		printsi('#',predicate ,className , name , parameters)

		printl(string())
		printai("ERROR invalid name", name,"#",s)
	}

	return true;
}

std::string FunctionInfo::js() {
	VString v;
	std::string s,p;
	if (!className.empty()) {
		s = className + "::";
	}

	p="";
	std::size_t pos=0;
	//assume comment[i+1].second > comment[i].second
	for(auto a:comment){
		auto b=a.second;
		p+=toHTML(parameters.substr(pos, b-pos))+spanSurround(toHTML(a.first,false),"c");
		pos=b;
	}
	p+=toHTML(parameters.substr(pos));

	JS2("p",
			toHTML(predicate) + " " + spanSurround(s + name, "b")
					+ p)

	JS2("fileline", jsFileLine(file, line))

	/* column for sorting of 'p' column
	 * always add 's' suffix at the end so got 'ps'
	 * should be added at the end of js object
	 */
	JS2("ps", name)

	return surround(jc(v), CURLY);

}
#ifndef NDEBUG
std::string FunctionInfo::string() {
	VString v;
	std::string s;
	if (!className.empty()) {
		s = className + "::";
	}

	return predicate + " " + s + name + parameters + " " + file + " "
			+ std::to_string(line);
}

std::string FunctionInfo::fullString(){
#define A(a) #a,'=',a,"\n"
	return formatz(A(predicate),A(className),A(name),A(parameters),A(file),A(line));
#undef A
}

#endif

bool FunctionInfo::isValidItem(std::string const& s) {
	return ONE_OF(s,VALID) || startsWith(s, TEMPLATE) || s == CONST + "*"
			|| s == CONST + "&";
}

bool FunctionInfo::getItem(std::string const& s, std::size_t& pos,
		std::string& r) {
	//use signed int for cycle for normal check f>=0, f1>=0
	int f = pos, f1;
	std::string q,w;

	for (f--; f >= 0 && isspace(s[f]); f--)
		;
	if (f <= 0 || s[f] == ';') {
		return false;
	}

	//never happens
	if(isLessPrecede(s,f)){
		q = s.substr(0, f);
		f1 = getBalanceBracketsPos(q, ANGLE);
		q = q.substr(f1 + 1, f - f1) + ">";
//		printl(s)
//		printl(q)
		f = f1;
		printi
	}

	//use for operators +-*/!
	for (f1 = f - 1; f1 >= 0; f1--) {
		if (isalnum(s[f1]) || strchr(":_#~+-*/=!", s[f1])) {
			continue;
		}

		if(isLessPrecede(s,f1)){
			f1 = getBalanceBracketsPos(s.substr(0, f1), ANGLE);
			continue;
		}

		break;
	}

	pos = f1;
//	printl('=',s.substr(f1 + 1, f - f1),'=', q)
	r = s.substr(f1 + 1, f - f1) + q;
	return true;
}

bool FunctionInfo::isLessPrecede(std::string const& s,int&pos){
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
