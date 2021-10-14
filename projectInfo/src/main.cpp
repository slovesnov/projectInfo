/*
 * main.cpp
 *
 *  Created on: 09.12.2017
 *      Author: alexey slovesnov
 */
#include <cstdio>
#include <fstream>
#include <filesystem>
using namespace std::filesystem;

#include "help.h"
#include "ProjectInfo.h"

/* -1 - test mode
 * >=0 - {path,oneProject,outputFileName} from projectOption
 */
#define TYPE 2

std::tuple<std::string,bool,std::string> projectOption[]={
		/*0*/{"c:/downloads/1",1,"out"}
		/*1*/,{"c:/Users/user/git/projectInfo/projectInfo/src",1,"projectInfo"}
		/*2*/,{"c:/Users/user/git/bridge/bridge/src",1,"bridge"}
		/*3*/,{"c:/Users/user/git/aslov/aslov/src/aslov",1,"aslov"}
};

int main(int argc, char* argv[]) {
	static_assert(TYPE==-1 || (TYPE>=0 && TYPE<=SIZEI(projectOption)) );
#if TYPE==-1

	//int i=0;
	std::string s= {"<class B : public virtual A "};

	std::string B = "\\s*((public|protected|private)(\\s+virtual)?\\s+)?([\\w:<>]+)\\s*";

	std::regex r(
			"\\b(class|struct|union)\\s+(\\w+)\\s*(:" + B + "(," + B + ")*)?$");

	std::smatch match;
	std::string q;
	VString v;
	PStringString c;

	if (regex_search(s, r)) {
		printl("found");
	}
	else {
		printl("not found");

	}

	std::regex r1(
			"(^|[^\\w<])(class|struct|union)\\s+(\\w+)\\s*(:" + B + "(," + B + ")*)?$");
	if (regex_search(s, r1)) {
		printl("found");
	}
	else {
		printl("not found");

	}

#else
	clock_t begin = clock();
	std::string s, s1;
	VProjectInfo v;
	VString w;
	int i;
	bool proceedFunctions = true;
	auto&a=projectOption[TYPE];
	std::string root=std::get<0>(a);
	bool oneProject=std::get<1>(a);
	std::string htmlName=std::get<2>(a);

	if (argc > 1) {
		i = 1;
		root = argv[i++];
		oneProject = std::stoi(argv[i++]);
		proceedFunctions = std::stoi(argv[i++]);
		htmlName="out";
	}

	if (!oneProject) {
		proceedFunctions = false;
	}

	const bool deleteSkipFiles = false;

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
		printf("functions %lld ", v.m_fi.size());

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

	printf("the end removed files %d\ntime %.2lf(s)", ProjectInfo::removedFiles(),
			timeElapse(begin));

#endif
}
