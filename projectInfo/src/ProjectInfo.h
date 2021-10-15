/*
 * projectinfo.h
 *
 *  Created on: 09.12.2017
 *      Author: alexey slovesnov
 */

#ifndef PROJECTINFO_H_
#define PROJECTINFO_H_

#include <set>
#include <ctime>

#include "FileInfo.h"
#include "ContentInfo.h"

class ProjectInfo {
	static std::string m_root;
	static bool m_deleteSkipFiles;
	static bool m_proceedFunctions;
	static int m_removed;
	static std::set<std::string> m_skipExtension;
	static std::set<std::string> m_proceedExtension;

	VFileInfo m_vsi;
	int m_size; //total files size
	int m_lines;
	std::string m_name;

public:
	VFunctionInfo m_fi;
	VClassInfo m_ci;
	VContentInfo m_vci;

	//if deleteSkipFiles is true then remove all files which is skipped
	static void staticInit(std::string const& root, bool proceedFunctions,
			bool deleteSkipFiles);

	ProjectInfo(const std::string& path);

	int files() const {
		return m_vsi.size();
	}

	int size() const {
		return m_size;
	}

	int lines() const {
		return m_lines;
	}

	auto& name() const {
		return m_name;
	}

	int fsize() const {
		return m_fi.size();
	}

	static int removedFiles() {
		return m_removed;
	}

	bool operator<(ProjectInfo const& p) const {
		return m_size > p.m_size;
	}

	std::string jsFileData(bool oneProject);

	void postProceedFunctions();

	std::string jsFunctionsData();
	std::string jsClassData();
};
DECLARE_VECTOR_TYPE(ProjectInfo)

#endif /* PROJECTINFO_H_ */
