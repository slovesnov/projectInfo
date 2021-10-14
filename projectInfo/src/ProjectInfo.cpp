/*
 * projectinfo.cpp
 *
 *  Created on: 09.12.2017
 *      Author: alexey slovesnov
 */
#include <cassert>
#include <fstream>

#include "ProjectInfo.h"
#include "FunctionInfo.h"
#include "ClassInfo.h"

std::string ProjectInfo::m_root;
bool ProjectInfo::m_deleteSkipFiles;
bool ProjectInfo::m_proceedFunctions;
int ProjectInfo::m_removed;

std::set<std::string> ProjectInfo::m_skipExtension = {
		"",
		"aps",
		"bmp",
		"cfg",
		"classpath",
		"clw",
		"cproject",
		"css",
		"db",
		"doc",
		"dsp",
		"dsw",
		"gif",
		"h-",
		"htm",
		"html",
		"ico",
		"iss",
		"jpg",
		"ln",
		"lng",
		"ncb",
		"old",
		"opt",
		"plg",
		"png",
		"prefs",
		"project",
		"rar",
		"rc",
		"rc2",
		"txt",
		"zip",
		"csv",
		"log" };

std::set<std::string> ProjectInfo::m_proceedExtension = {
		"_cpp",
		"c",
		"cpp",
		"h",
		"java",
		"js",
		"php" };

void ProjectInfo::staticInit(std::string const& root, bool proceedFunctions,
		bool deleteSkipFiles) {
	m_root = root;
	m_proceedFunctions = proceedFunctions;
	m_deleteSkipFiles = deleteSkipFiles;
	m_removed = 0;
}

ProjectInfo::ProjectInfo(const std::string& path) {
	int i;
	const char*b;
	std::string e, s;
	const bool one = path == m_root;

	if (one) {
		std::size_t f = path.rfind('\\');
		if (f == std::string::npos) {
			f = path.rfind('/');
		}
		m_name = path.substr(f + 1);
	}
	else {
		m_name = path.substr(m_root.length() + 1);
	}

	for (auto& p : recursive_directory_iterator(path)) {
		if (!one) {
			if (is_directory(p)) {
				continue;
			}
		}
		s = p.path().string();
		e = getFileInfo(s,FILEINFO::LOWER_EXTENSION);
		if (m_skipExtension.find(e) != m_skipExtension.end()) {
			if (m_deleteSkipFiles) {
				if (remove(p)) {
					m_removed++;
				}
				else {
					printf("error remove file[%s]\n", s.c_str());
				}
			}
			continue;
		}

		b = s.c_str() + m_root.length() + 1 + (one ? 0 : m_name.length() + 1);

		if (m_proceedExtension.find(e) != m_proceedExtension.end()) {
			std::ifstream filein(s);
			std::string line;
			for (i = 0; getline(filein, line); i++)
				;
			auto ftime = last_write_time(p);
			std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
			m_vsi.push_back( { b, int(file_size(p)), i, cftime });

			if (m_proceedFunctions) {
				proceedFunctions(s, b);
			}

		}
		else {
			printf("error unknown extension [%s] file [%s]\n", e.c_str(), b);
		}

	}

	m_size = 0;
	m_lines = 0;
	for (auto a : m_vsi) {
		m_size += a.size;
		m_lines += a.lines;
	}

	if (m_proceedFunctions) {
		postProceedFunctions();
	}

	/*
	 auto o1=[](FileInfo const& a, FileInfo const& b) {return a.size>b.size;};
	 auto o2=[](FileInfo const& a, FileInfo const& b) {return a.lastModification>b.lastModification;};

	 if (m_orderType == 0) {
	 sort(m_vsi.begin(), m_vsi.end(),o1);
	 }
	 else {
	 sort(m_vsi.begin(), m_vsi.end(),o2);
	 }
	 */
}

std::string ProjectInfo::jsFileData(bool oneProject) {
	VString v, w;

	if (oneProject) {
		for (auto& t : m_vsi) {
			w.push_back(t.js());
		}
		return jc(w);
	}

#define A(a) v.push_back(js2(#a, a()));
	A(name)
	A(files)
	A(size)
	A(lines)
#undef A

	for (auto& t : m_vsi) {
		w.push_back(t.js());
	}

	v.push_back(js2("a", jc(w), SQUARE));
	return surround(jc(v), CURLY);
}

void ProjectInfo::proceedFunctions(std::string const& file,
		std::string const& fileName) {
	int i, j, line, curly;
	std::string s, e, q;
	std::size_t f;
	VString classes,v;
	ClassInfo ci;
	FunctionInfo fi;
	VPStringSize vp, vp1;
	int64_t k, d,la;
	VPStringSize::const_iterator sit,it;

	std::ifstream t(file);
	std::stringstream buffer;
	buffer << t.rdbuf();
	s = buffer.str();

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
	v = splitr(s, r+OR+COMMENT);
	s="";
	i=0;
	for(auto const&a:v){
		if (regex_search(a, std::regex("^" + COMMENT))) {
			vp.push_back({a,i});
			//printl(i,a)
			//printl(i)
		}
		else{
			s+=a;
			i+=a.length();
		}
	}

//	printl("["+s+"]",s.length())
//	printl("["+s.substr(23)+"]")

//	q=s;
//	for(auto it=vp.rbegin();it!=vp.rend();it++){
//		auto a=it->second;
//		q=q.substr(0, a)+it->first+q.substr(a);
//	}
//	printl(q)



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

		//println("[%s]%d",a.c_str(),line)

		if (a == "{") {
			//Note some of v[..] could be empty
			s = i == 0 ? "" : v[i - 1];
			if (!s.empty()) {
				if (ci.check(s, classes, curly, fileName, line)) {
					classes.push_back(ci.name);
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
			if (m_ci.find(ci.name) != m_ci.end()) {
				auto& q = m_ci.find(ci.name)->second;
				println("ERROR %s %s:%d [%s:%d]", ci.name.c_str(), q.file.c_str(),
						q.line, fileName.c_str(), line)
			}
			else {
				m_ci[ci.name] = ci;
			}
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
		 =>
		 AboutDialog::AboutDialog()

		 //also this test later
		 public:
		 void resize(int n, int _k) {

		 */
		j = f;
		for (j = f; j >= 0 && s[j] != ':'; j--)
			;

		q = s.substr(0, j);

		/* s[j - 1] == ':'  checks this "bool Base::selectColor(const char* s, GdkRGBA* color)"
		 * when describe class member s[j]==':' && s[j-1]==':'
		 */

		if (j >= 1 && s[j - 1] != ':'
				&& !regex_search(q, std::regex("\\b(public|private|protected)\\s*$"))) {

			if (!regex_search(q, std::regex("\\)\\s*$"))) {
				printl("strange string", q)
			}

			if (pf(q, s, e, f)) {
				goto l337;
			}
		}

		if (fi.check(s, f, e, classes, curly, fileName, line,vp1)) {
			m_fi.push_back(fi);
		}
		else{
		}

l337:
	i++;
	line+=countLines(a);
	}
}

void ProjectInfo::postProceedFunctions() {
	int i, j, k, l, sz;
	std::string s;
	ClassInfo ci;
	VString v;

	//calculates inheritance table
	for (auto&a : m_ci) {
		auto&o = a.second;
		v.push_back(o.name);
	}

	sz = m_ci.size();
	bool*c = new bool[sz * sz];
	for (i = 0; i < sz * sz; i++) {
		c[i] = false;
	}
	for (auto&a : m_ci) {
		auto&o = a.second;
		i = indexOf(o.name,v );
		assert(i != -1);
		for (auto&b : o.base) {
			s = b.second;
			//std::vector<int>
			if (s.find("std::") != std::string::npos) {
				continue;
			}
			j = indexOf(s,v);
			if (j == -1) {
				printl("ERROR no class declaration found", s);
				continue;
			}
			assert(j != i);
			c[i * sz + j] = true;
		}
	}

	//c[child*sz+base]
	int* cd = new int[sz];
	int* ca = new int[sz];

	l = 0;
	for (i = 0; i < sz; i++) {
		k = 0;
		for (j = 0; j < sz; j++) {
			if (c[j * sz + i]) {
				k++;
			}
		}
		ca[i] = k == 0 ? 0 : -1;
		cd[i] = k;
		if (k) {
			l++;
		}
	}

	while (l) {
		for (i = 0; i < sz; i++) {
			if (ca[i] != -1) {
				continue;
			}

			k = 0;
			for (j = 0; j < sz; j++) {
				if (c[j * sz + i]) {
					if (ca[j] == -1) {
						break;
					}
					else {
						k += ca[j];
					}
				}
			}

			if (j == sz) {
				l--;
				ca[i] = k + cd[i];
			}
		}
	}

	i = 0;
	for (auto&a : m_ci) {
		auto&o = a.second;
		o.childDirectly = cd[i];
		o.childAll = ca[i];
		i++;
	}

	delete[] c;
	delete[] ca;
	delete[] cd;

	ci.childDirectly = ci.childAll = ci.functions = 0;
	m_ci[""] = ci;

	for (auto&a : m_fi) {
		auto it = m_ci.find(a.className);
		if (it == m_ci.end()) {
			printl("ERROR no class declaration found", a.className)
		}
		else {
			it->second.functions++;

		}
	}

}

std::string ProjectInfo::jsFunctionsData() {
	VString v;
	for (auto&a : m_fi) {
		v.push_back(a.js());
	}
	return jc(v);
}

std::string ProjectInfo::jsClassData() {
	VString v;
	for (auto&a : m_ci) {
		v.push_back(a.second.js());
	}
	return jc(v);
}

bool ProjectInfo::pf(std::string const& a, std::string& s, std::string& e,
		std::size_t& f) {
	std::smatch match;
	std::regex r(R"(\)\s*(const)?\s*$)");
	if (!regex_search(a, match, r)) {
		return true;
	}

	s = a.substr(0, match.position(0));
	e = match.str(0);

	f = getBalanceBracketsPos(s, ROUND);
	return f == std::string::npos;
}
