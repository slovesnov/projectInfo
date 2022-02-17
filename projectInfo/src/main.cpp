/*
 * main.cpp
 *
 *  Created on: 09.12.2017
 *      Author: alexey slovesnov
 */

#include "aslov.h"
#include "help.h"
#include "ProjectInfo.h"

/* TYPE = -1 - proceed all projectName
 * TYPE = 0 .. SIZE(projectName)-1 - {path,oneProject,outputFileName} from projectOption
 * TYPE = SIZE(projectName) .. SIZE(projectName)+SIZE(projectOptions)-1
 */
#define TYPE -1

const std::string projectName[] = { /*0*/"projectInfo"
		, /*1*/"bridge"
		, /*2*/"aslov"
		, /*3*/"words"
		, /*4*/"calculator"
		, /*5*/"imageviewer"
};

const TStringBoolString projectOptions[] = {
		/*6*/{"c:/downloads/1",1,"out"}
};

int main(int argc, char* argv[]) {
	static_assert(TYPE==-1 || (TYPE>=0 && TYPE<SIZE(projectName)+SIZE(projectOptions)) );

	bool proceedFunctions = true;
	const bool deleteSkipFiles = false;
	TStringBoolString t;

#if TYPE==-1

	for(auto a:projectName){
		t=getProjectOptions(a);
		proceedDirectory(t,proceedFunctions,deleteSkipFiles,false);
	}

#else
	if (TYPE >= SIZE(projectName)) {
		t=projectOptions[TYPE-SIZE(projectName)];
	}
	else{
		t=getProjectOptions(projectName[TYPE]);
	}

	if (argc > 1) {
		//i = 1;
		//root oneProject htmlName
		t={ argv[1], std::stoi(argv[2]),argv[3] };
		proceedFunctions = std::stoi(argv[4]);
	}


	proceedDirectory(t,proceedFunctions,deleteSkipFiles,TYPE >= SIZE(projectName));

#endif
}
