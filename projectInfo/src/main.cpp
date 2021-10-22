/*
 * main.cpp
 *
 *  Created on: 09.12.2017
 *      Author: alexey slovesnov
 */

#include <filesystem>
using namespace std::filesystem;

#include "aslov.h"
#include "help.h"
#include "ProjectInfo.h"

/* TYPE = -1 - test mode
 * TYPE = 0 .. SIZE(projectName)-1 - {path,oneProject,outputFileName} from projectOption
 * TYPE = SIZE(projectName) .. SIZE(projectName)+SIZE(projectOptions)-1
 */
#define TYPE 2

const std::string projectName[] = { /*0*/"projectInfo"
		, /*1*/"bridge"
		, /*2*/"aslov"
		, /*3*/"words"
		,/*4*/"calculator"
		, /*5*/"imageviewer"
};

const TStringBoolString projectOptions[] = {
		/*6*/{"c:/downloads/1",1,"out"}
};

int main(int argc, char* argv[]) {
	static_assert(TYPE==-1 || (TYPE>=0 && TYPE<SIZE(projectName)+SIZE(projectOptions)) );
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
	std::string s, s1,root,htmlName;
	VProjectInfo v;
	VString w;
	int i;
	bool proceedFunctions = true,oneProject;
	TStringBoolString t;
	if (TYPE >= SIZE(projectName)) {
		t=projectOptions[TYPE-SIZE(projectName)];
	}
	else{
		t=getProjectOptions(projectName[TYPE]);
	}
	root=std::get<0>(t);
	oneProject=std::get<1>(t);
	htmlName=std::get<2>(t);

	if (TYPE >= SIZE(projectName)) {
		printzn("proceed dir ",root," to ",htmlName,".html");
	}
	else{
		printan("proceed",htmlName);
	}
	fflush(stdout);

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

	printan("the end removed files", ProjectInfo::removedFiles());
	printzn("time ",timeElapse(begin),"(s)");

#endif
}
