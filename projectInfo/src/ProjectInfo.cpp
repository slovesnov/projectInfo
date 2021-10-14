/*
 * projectinfo.cpp
 *
 *  Created on: 09.12.2017
 *      Author: alexey slovesnov
 */
#include <thread>
#include <fstream>

#include "ProjectInfo.h"

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
	std::string e, s, q, localPath;
	std::vector<std::thread> vt;
	const bool one = path == m_root;
	const int cores=1;//getNumberOfCores();

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

		localPath = s.substr( m_root.length() + 1 + (one ? 0 : m_name.length() + 1) );

		if (m_proceedExtension.find(e) != m_proceedExtension.end()) {
			q=fileGetContent(s);
			auto ftime = last_write_time(p);
			std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
			m_vsi.push_back( { localPath, int(file_size(p)), countLines(q)+1, cftime });

			if (m_proceedFunctions) {
				m_vcontentFile.push_back({q,localPath});
			}

		}
		else {
			//printf("error unknown extension [%s] file [%s]\n", e.c_str(), name);
			printl("error unknown extension ["+e+"] file ["+localPath+"]");
		}

	}

	for (i=0; i<cores; ++i){
	    vt.push_back(std::thread(proceed,i,this));
		m_vci.push_back({});
	}

	m_fi=m_vci[0].m_fi;
	m_ci=m_vci[0].m_ci;

	for (auto& a : vt){
		a.join();
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
