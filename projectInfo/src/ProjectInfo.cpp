/*
 * projectinfo.cpp
 *
 *  Created on: 09.12.2017
 *      Author: alexey slovesnov
 */
#include <thread>
#include <atomic>
#include <sys/stat.h>
#include <filesystem>
using namespace std::filesystem;

#include "aslov.h"
#include "ProjectInfo.h"

std::string ProjectInfo::m_root;
bool ProjectInfo::m_deleteSkipFiles;
bool ProjectInfo::m_proceedFunctions;
int ProjectInfo::m_removed;

const bool ONECORE=0;

extern std::atomic<int> gFileNumber;

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
	std::string e, s, content, localPath;
	std::vector<std::thread> vt;
	const bool one = path == m_root;
	const int cores=ONECORE?1:getNumberOfCores();
//	clock_t begin = clock();

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
					printan("error remove file",s);
				}
			}
			continue;
		}

		localPath = s.substr( m_root.length() + 1 + (one ? 0 : m_name.length() + 1) );

		if (m_proceedExtension.find(e) != m_proceedExtension.end()) {
			content=fileGetContent(s);

			/*
			 *
			auto ftime = last_write_time(p);
			std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
			//this works only if use
			#include <experimental/filesystem>
			using namespace std::experimental::filesystem;
			and add option to linker library "stdc++fs"

			//if use
			#include <filesystem>
			using namespace std::filesystem;
			need to use code, but it returns different results +/- 1 second so it's very difficult to debug program
			template <typename TP>
			std::time_t to_time_t(TP tp)
			{
				using namespace std::chrono;
				auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
						  + system_clock::now());
				return system_clock::to_time_t(sctp);
			}

			so the solution is use old stat() function
*/

			struct stat result;
			stat(s.c_str(), &result);
			auto mod_time = result.st_mtime;

			//assert(cftime==ct);
			m_vsi.push_back( { localPath, int(file_size(p)), countLines(content)+1, mod_time });

			if (m_proceedFunctions) {
				m_vci.push_back({localPath,content});
			}

		}
		else {
			printan("error unknown extension",e,"file",localPath);
		}

	}

//	println("reading time %.2lf(s)",timeElapse(begin));
//	begin = clock();


	gFileNumber=m_vci.size()-1;

//	sort(m_vci.begin(), m_vci.end());
//	for(auto & a:m_vci){
//		printl(a.file,a.content.length());
//	}
//	fflush(stdout);

	for (i=0; i<cores; ++i){
	    vt.push_back(std::thread(proceed,i,&m_vci));
	}

	for (auto& a : vt){
		a.join();
	}
//	println("proceed time %.2lf(s)",timeElapse(begin));

	for(auto & a:m_vci){
		auto&b=a.m_fi;
		m_fi.insert(m_fi.end(), b.begin(), b.end());

		auto&c=a.m_ci;
		m_ci.insert(m_ci.end(), c.begin(), c.end());
	}

	m_size = 0;
	m_lines = 0;
	for (auto a : m_vsi) {
		m_size += a.size;
		m_lines += a.lines;
	}

	if (m_proceedFunctions) {
		postProceedFunctions();

		//after postProceedFunctions because empty class (global functions is added)
		sort(m_ci.begin(), m_ci.end());
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

void ProjectInfo::postProceedFunctions() {
	int i, j, k, l, sz;
	std::string s;
	ClassInfo ci;
	VString v;
	std::set<std::string> set;

	//calculates inheritance table
	for (auto&a : m_ci) {
		s=a.name;
		v.push_back(s);
		if(set.find(s)!=set.end()){
			printan("ERROR class",s,"is defined more than one time");
		}
		set.insert(s);
	}

	sz = m_ci.size();
	bool*c = new bool[sz * sz];
	for (i = 0; i < sz * sz; i++) {
		c[i] = false;
	}
	for (auto&a : m_ci) {
		i = indexOf(a.name,v );
		assert(i != -1);
		for (auto&b : a.base) {
			s = b.second;
			//std::vector<int>
			if (s.find("std::") != std::string::npos) {
				continue;
			}
			j = indexOf(s,v);
			if (j == -1) {
				printan("ERROR no class declaration found", s);
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
		a.childDirectly = cd[i];
		a.childAll = ca[i];
		i++;
	}

	delete[] c;
	delete[] ca;
	delete[] cd;

	ci.childDirectly = ci.childAll = ci.functions = 0;
	ci.name="";
	m_ci.push_back(ci);

	for (auto&a : m_fi) {
		auto& cl=a.className;
		auto it = std::find_if(m_ci.begin(), m_ci.end(),
				[&cl](const ClassInfo &c) {
					return c.name == cl;
				}
		);
		if (it == m_ci.end()) {
			printan("ERROR no class declaration found", a.className)
		}
		else {
			it->functions++;
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
		v.push_back(a.js());
	}
	return jc(v);
}
