/*
 * common.cpp
 *
 *       Created on: 26.12.2020
 *           Author: aleksey slovesnov
 * Copyright(c/c++): 2020-doomsday
 *           E-mail: slovesnov@yandex.ru
 *         Homepage: slovesnov.users.sourceforge.net
 */


#include <filesystem>
using namespace std::filesystem;
#include <atomic>

#include "help.h"
#include "ProjectInfo.h"

std::atomic_int gFileNumber;

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
		"final",//add 14oct21
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
		"override",//add 14oct21
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

std::string toHTML(const std::string& s,bool withKeywords/*=true*/) {
	int i;
	std::string s1;
	// "'" -> "&#39;"
	//"void f(char c/*=','*/)" single quote inside js string so replace quote
	//
	// "\\" -> &#92;
	//"void f(char c/*='\n'*/)" backslaskh inside js string so replace quote

	const std::string FROM[] = { "&", "<", ">", "\n", "\t","'","\\" };
	const std::string TO[] = { "&amp;", "&lt;", "&gt;", "<br>", "  ","&#39;","&#92;" };
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

	if(withKeywords){
		for (auto a : KEYWORDS) {
			s1 = replaceAllr(s1, "\\b" + a + "\\b", spanSurround(a, "k"));
		}
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

bool pf(std::string const& a, std::string& s, std::string& e,
		std::size_t& f) {
	std::smatch match;
	std::regex r(R"(\)\s*(const)?(\s+(final|override|noexcept|throw\s*\(\s*\))){0,3}\s*$)");
	if (!regex_search(a, match, r)) {
		return true;
	}

	s = a.substr(0, match.position(0));
	e = match.str(0);

	f = getBalanceBracketsPos(s, ROUND);
	return f == std::string::npos;
}

void proceedFile(ContentInfo& coi){
	int i, line, curly;
	std::string s, e, q;
	std::size_t p, f,lf;
	VString classes,v;
	ClassInfo ci;
	FunctionInfo fi;
	VPStringSize vp, vp1;
	int64_t k, d,la;
	VPStringSize::const_iterator sit,it;
	const std::string fileName=coi.file;

	/* splitters { or } or single line comment or multiline comment
	 * or string constant
	 * can has empty strings it's ok when one separator followed by another
	 * separator
	 */
	const std::string COMMENT(
			R"((//[^\n]*\n)|(\/\*[\s\S]*?\*\/))");
	//https://en.wikipedia.org/wiki/String_literal
	const std::string STRING=R"("(\\.|[^\\"])*")";
	const std::string CHAR = R"('(\\.|[^\\'])')";
	const std::string OR = "|";
	const std::string r(R"(\{|\})" + OR + STRING + OR + CHAR);

	//remove comments DO FULL SPLIT don't need comments inside strings
	v = splitr(coi.content, r+OR+COMMENT);
	s="";
	i=0;
	for(auto const&a:v){
		if (regex_search(a, std::regex("^" + COMMENT))) {
			vp.push_back({a,i});
		}
		else{
			s+=a;
			i+=a.length();
		}
	}

	//\b void meanwhile(){}
	const std::regex BLOCK(R"(\b(for|if|while|catch|switch)\s*$)");

	curly = 0;
	i = 0;
	line=1;
	v = splitr(s, r);
	k=0;
	sit=vp.begin();
	for (auto const&a:v) {

		//adjust comment new lines
		la = a.length();
		vp1.clear();
		for (it = sit; it != vp.end(); it++) {
			auto const &b = *it;
			d = (int64_t)(b.second) - k;
			if(d >= la){
				sit=it;
				break;
			}
			if (d>=0 ) {
				vp1.push_back( { b.first, d });
				line += countLines(b.first);
			}
		}
		k += la;

		if (a == "{") {
			//Note some of v[..] could be empty
			s = i == 0 ? "" : v[i - 1];
			if (!s.empty()) {
				if (ci.check(s, classes, curly, fileName, line)) {
					if(curly<int(classes.size())){
						classes[curly]=ci.name;
					}
					else{
						classes.push_back(ci.name);
					}
				}
				else {
				}
			}
			curly++;
			goto l337;
		}
		else if (a == "}") {
			curly--;
			goto l337;
		}
		else if (a[0] == '"' || a[0] == '\'') {
			goto l337;
		}

		//proceed only if next lexeme is '{'
		if (i == int(v.size() - 1)) {
			break;
		}
		if (v[i + 1] != "{") {
			goto l337;
		}

		/* get class inheritance ONLY IF next "{" because
		 * "class Frame;" is not declaration
		 */
		if (ci.check(a, classes, curly, fileName, line)) {
			coi.m_ci.push_back(ci);
		}

		if (pf(a, s, e, f)) {
			goto l337;
		}

		q = s.substr(0, f);

		if (regex_search(q, BLOCK)) {
			goto l337;
		}

		/*
		AboutDialog::AboutDialog() :
		BaseDialog(MENU_ABOUT) {
		=> AboutDialog::AboutDialog()

		Exception(const char *message):std::exception()
		=> Exception(const char *message)

		//also this test later
		public:
		void resize(int n, int _k) {
		 */
		lf=f;
		while ((p = s.find_last_of(':', f)) != std::string::npos) {
			if(p==0){
				printl("strange string");
				break;
			}
			f=p-1;
			if(s[p - 1] == ':'){
				f--;
				continue;
			}

			q = s.substr(0, p);

			/* s[p - 1] == ':'  checks this "bool Base::selectColor(const char* s, GdkRGBA* color)"
			 * when describe class member s[j]==':' && s[j-1]==':'
			 */
			//printzi('#',s.substr(0,f),"#",s.substr(f),"#",f)
			if (!regex_search(q, std::regex("\\b(public|private|protected)\\s*$"))) {
				if (!regex_search(q, std::regex("\\)\\s*$"))) {
					printzi("strange string[", q,"]")
				}
				if (pf(q, s, e, f)) {
					goto l337;
				}
				else{
					lf=f;
					break;
				}
			}
		}//while
		f=lf;

//		for (j = f; j >= 0 && s[j] != ':'; j--)
//			;
//
//		q = s.substr(0, j);
//
//		/* s[j - 1] == ':'  checks this "bool Base::selectColor(const char* s, GdkRGBA* color)"
//		 * when describe class member s[j]==':' && s[j-1]==':'
//		 */
//		printzi('#',s.substr(0,f),"#",s.substr(f),"#",f)
//		if (j >= 1 && s[j - 1] != ':'
//				&& !regex_search(q, std::regex("\\b(public|private|protected)\\s*$"))) {
//
//			if (!regex_search(q, std::regex("\\)\\s*$"))) {
//				printl("strange string", q)
//			}
//
//			printl(f)
//			if (pf(q, s, e, f)) {
//				goto l337;
//			}
//			printl(f)
//		}
//		printzi('#',s.substr(0,f),"#",s.substr(f),"#",e,"#",f)

		if (fi.check(s, f, e, classes, curly, fileName, line,vp1)) {
//			printl(fi.fullString())
			coi.m_fi.push_back(fi);
		}
		else{
		}

l337:
	i++;
	line+=countLines(a);
	}

}

void proceed(int n,VContentInfo* pv){
	int i;
	while( (i=gFileNumber--) >= 0 ){
		proceedFile((*pv)[i]);
	}
}

TStringBoolString getProjectOptions(const std::string &name) {
	bool b=1;
	auto s="c:/Users/user/git/"+name+"/"+name+"/src";
	if(name=="aslov"){
		s+="/"+name;
	}
	return {s,b,name};
}

void proceedDirectory(TStringBoolString const& t,bool proceedFunctions,bool deleteSkipFiles,bool ot){
	int i;
	std::string s,s1;
	clock_t begin = clock();
	VProjectInfo v;
	VString w;

	std::string root=std::get<0>(t);
	const bool oneProject=std::get<1>(t);
	std::string htmlName=std::get<2>(t);

	if (ot) {
		printzn("proceed dir ",root," to ",htmlName,".html");
	}
	else{
		printan("proceed",htmlName);
	}
	fflush(stdout);

	if (!oneProject) {
		proceedFunctions = false;
	}

	ProjectInfo::staticInit(root, proceedFunctions, deleteSkipFiles);

	s = "<html><head><script src='ps.js'></script><link rel='stylesheet' href='ps.css'><script>";


	if (oneProject) {

		ProjectInfo v(root);

		for (i = 0; i < 3; i++) {
			if (i == 0) {
				s1 = v.jsFileData(true);
			}
			else if (i == 1) {
				s1 = v.jsClassData();
			}
			else {
				s1 = v.jsFunctionsData();
			}
			s += "g" + std::to_string(i) + "=" + surround(s1, SQUARE) + ";";

		}

		s += "</script></head><body onload='loadf()'>";
		s += "<div class='tab' id='d'></div><span id='s'></span>";
		printz("functions ",v.m_fi.size()," ");

	}
	else {
		for (auto& p : directory_iterator(root)) {
			if (is_directory(p)) {
				v.push_back(ProjectInfo(p.path().string()));
			}
		}
		sort(v.begin(), v.end());

		for (auto& t : v) {
			w.push_back(t.jsFileData(false));
		}
		s += "gd=" + surround(jc(w), SQUARE) + ";";

		s += "</script></head><body onload='load()'><table id='";
		s += oneProject ? "table0" : "main";
		s += "'></table>";
	}
	s += "</body></html>";

	std::ofstream f(htmlName+".html");
	f<<s;
	if(deleteSkipFiles){
		printan("the end removed files", ProjectInfo::removedFiles());
	}
	printzn("time ",timeElapse(begin),"(s)");

}
