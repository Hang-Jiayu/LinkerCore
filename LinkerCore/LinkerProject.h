#pragma once
#include "LinkerCore.h"

namespace LinkerProject
{

	class Linker
	{
	public:
		QJsonObject MainPath;
		QJsonObject Settings;
		QJsonObject Paths;
		void init()
		{
			//��Ŀ��·��
			MainPath.insert("Parent", "Linker");
			MainPath.insert("Child", "Linker");
			//��Ŀ����·��
			Paths.insert("0", "/log");
			Paths.insert("1", "/plugins");
			Paths.insert("2", "/plugins/Language");
			Paths.insert("3", "/users");
			//��Ŀ����
			Settings.insert("Project", "Linker");
			Settings.insert("Version", "1.0.0.1");
			Settings.insert("UserFilePath", "default");
			Settings.insert("Theme", "default");
			Settings.insert("Language", "default");
			Settings.insert("SelfStart", false);
		}
	};


}

