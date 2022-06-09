#pragma once
#include <string>
#include <iostream>


#include <qdebug.h>
#include <qstandardpaths.h>
#include <qdir.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>

/*
* [������������]
* #1.1 int����
* 1.return 0
* û�����⣬��������
* 2 reuturn 1
* ��������������������δִ�У����������ܣ�
* 3 return -1
* ���й���
* 
* 
* 
*/





namespace LinkerCore
{
	class LinkerCore
	{
	private:
		bool inited = false;
	public:
		std::string project;
		int inited_result = -3;
		bool inited_setting = false;
		//�����ļ���·��
		QString MainPath;
		//config.json·��
		QString ConfigPath;
		//ͨ���ļ�����
		QFile File;
		//config�ļ�
		QFile file_config;
		//ͨ��·������
		QDir Dir;
		//��Ŀ·����
		QJsonObject Pro_path;
		//��Ŀ��ʼ����
		QJsonObject Settings;
		//��Ŀ��ǰ����
		QJsonObject Current_settings;
		//��Ŀ����·��
		QJsonObject Paths;
		//Json�ĵ�
		QJsonDocument Jsdoc;

		//��ʼ����
		int init(QJsonObject pro_path);
		//��Ŀ����
		int init_settings(QJsonObject settingsl, bool confirm);
		//�����û�
		int reg(QString username, QString psw);
		//��ȡ����
		QString get_setting(QString key);
		


	};
}


//��ʼ��
int LinkerCore::LinkerCore::init(QJsonObject pro_path)

{
	if (inited == false)
	{
		//�����ļ�����
		
		
		try
		{
			//�����ļ���·��
			MainPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/";
			MainPath.append(pro_path.value("Parent").toString());
			MainPath.append("/");
			MainPath.append(pro_path.value("Child").toString());
			//config.json·��
			ConfigPath = MainPath.append("/config.json");
			//��ȡ��Ŀ�ļ�·��
			Pro_path = pro_path;
			//�����ļ�
			QFile file_config(ConfigPath);
			//���ļ�������
			if (!File.exists(ConfigPath))
			{
				
				try
				{
					//config�ļ���
					file_config.open(QIODevice::WriteOnly);
					if (inited_setting == true)
					{
						Jsdoc.setObject(Settings);
						file_config.write(Jsdoc.toJson());
						file_config.close();
						Current_settings = Jsdoc.object();
						inited_result = 0;
						inited = true;
						return 0;
					}

					inited = true;
					//��ʼ����û����Ŀ����
					inited_result = 1;
					file_config.close();
					inited = true;
					return 1;
				}
				catch (const std::exception&)
				{
					if (file_config.isOpen())
					{
						file_config.close();
					}
					inited_result = -1;
					return -1;
				}
				
			}
			else
			{
				file_config.open(QIODevice::ReadOnly);
				QByteArray config_data = file_config.readAll();
				Jsdoc = QJsonDocument::fromJson(config_data);
				Current_settings = Jsdoc.object();
			}
			inited = true;
		}
		catch (const std::exception&)
		{
			
			//��ʼ���쳣
			inited_result = -1;
			return -1;
		}
		
	}
	//��ʼ���ɹ�/�ѳ�ʼ��
	inited_result = 0;
	inited = true;
	return 0;
}
//��ʼ������
int LinkerCore::LinkerCore::init_settings(QJsonObject settings,bool confirm=false)
{
	Settings = settings;
	inited_setting = true;
	if (confirm == false)
	{
		//��ʼ����û��д��
		return 1;
	}
	else
	{
		try
		{
		file_config.open(QIODevice::WriteOnly);
		Jsdoc.setObject(Settings);
		file_config.write(Jsdoc.toJson());
		file_config.close();
		}
		catch (const std::exception&)
		{
			if (file_config.isOpen())
			{
				file_config.close();
			}
			return -1;
		}

		return 0;
	}
	return 0;
}
//��ȡ����
QString LinkerCore::LinkerCore::get_setting(QString key)
{
	if (inited)
	{
		return Current_settings.value(key).toString();
	}
	else
	{
		return "";
	}
}
//�����˺�
int LinkerCore::LinkerCore::reg(QString username, QString psw = "")
{
	if (psw == "")
	{
		return 0;
	}
	return 0;
}
