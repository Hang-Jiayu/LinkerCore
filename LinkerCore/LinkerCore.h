#pragma once

/*
** (C) Copyright 2022 Hang Jiayu. All Rights Reserved.
** Description:����Qt�������̨����ģ��
** Author:Hang Jiayu
** Date:2022.07.28
** Modify Record:���ƻ�������
*/

#include <string>
#include <iostream>

#include <qdatastream.h>
#include <qdebug.h>
#include <qstandardpaths.h>
#include <qdir.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <quuid.h>
#include <qcryptographichash.h>
#include <qtextcodec.h>
#include <QtSql\qsqldatabase.h>
#include <QtSql\qsqlerror.h>
#include <QtSql\qsqlquery.h>
#include <QtSql\qsqlrecord.h>
#include <qdatetime.h>
#include <qtextstream.h>



class LinkerCore
{
private:
	//�Ƿ��ʼ��
	bool inited = false;
	//�Ƿ��¼
	bool logined = false;
	//�Ƿ��ʼ������
	bool inited_setting = false;
	
	//�Ƿ��¼��־��Ĭ�Ͽ���
	bool Logging = true;

	int _remove_user(std::string Username, std::string GroupName)
	{
		/*
		[����ֵ˵��]
		1.<0>:��������
		2.<-1>:δ�ɹ������ݿ�
		3.<-2>:δ��ʼ��
		4.<-3>:�����ڸ��û�
		5.<-4>:δ�ɹ�ɾ�����û�
		6.<-5>:δ�ɹ�ɾ����¼
		*/
		if (inited == false)
		{
			return -2;
		}
		//�����û��б�
		get_groupusers(GroupName);
		QString groupname = GroupName.c_str();
		QString username = QString::fromLocal8Bit(Username.c_str());
		if (groupuser_list.contains(username) == false)
		{
			return -3;
		}
		//�ж��Ƿ����UserGroup����
		if (QSqlDatabase::contains("UserGroup"))
		{
			db = QSqlDatabase::database("UserGroup");
		}
		else
		{
			db = QSqlDatabase::addDatabase("QSQLITE", "UserGroup");
			db.setDatabaseName(UserFilePath + "/UserGroup.lsf");
		}
		//�����ݿ�
		db.open();
		if (!db.isOpen())
		{
			//δ���û����ݿ�����
			return -1;
		}
		QSqlQuery sql_query(db);
		if (!sql_query.exec("delete from " + groupname + " where Name='" + username + "' and UUID='" + groupuser_list.value(username).toString() + "'"))
		{
			//qDebug() << sql_query.lastError() << endl;
			db.close();
			return -4;
		}
		db.close();
		get_users();
		if (user_list.contains(username) == false)
		{
			return 0;
		}
		/************ɾ���û����¼************/
		{
			if (QSqlDatabase::contains("Users"))
			{
				db = QSqlDatabase::database("Users");
			}
			else
			{
				db = QSqlDatabase::addDatabase("QSQLITE", "Users");
				db.setDatabaseName(UserFilePath + "/Users.lsf");
			}
			//�����ݿ�
			db.open();
			if (!db.isOpen())
			{
				//δ���û����ݿ�
				return -1;
			}
			QSqlQuery query(db);
			if (!query.exec("delete from " + user_list.value(username).toString() + " where GroupName='" + groupname + "'"))
			{
				//qDebug() << query.lastError() << endl;
				db.close();
				return -5;
			}
		}
		return 0;
	}


public:
	//��ʼ�����
	int inited_result = -10086;
	//���ݿ����
	QSqlDatabase db;
	//�����ļ���·��
	QString MainPath;
	//config.json·��
	QString ConfigPath;
	//�û��ļ�·��
	QString UserFilePath;
	//��־·��
	QString LogPath;
	//���·��
	QString PackagePath;
	//ͨ���ļ�����
	QFile File;
	//config�ļ�
	QFile file_config;
	QFile file_log;
	//ͨ��·������
	QDir Dir;
	//��Ŀ·����
	QJsonObject Pro_path;
	//��Ŀ��ʼ����
	QJsonObject Settings;
	//��Ŀ��ǰ����
	QJsonObject Current_settings;
	//Json�ĵ�
	QJsonDocument Jsdoc;
	//��ǰʱ�� 
	QDateTime curDateTime;

	//�ɺ��� get_users ��ֵ
	QJsonObject user_list;
	QJsonObject user_psw;

	//�ɺ��� get_groupusers ��ֵ[*��̬�仯]
	QJsonObject groupuser_list;
	QJsonObject usergroup_list;


	//��ʼ����
	int init(QJsonObject pro_path)
	{
		/*
		[����ֵ˵��]
		1.<0>:��������
		2.<-1>:δ�ɹ���config.json
		3.<-2>:δ֪����
		4.<-3>:δ�ɹ������û����ݿ�
		*/
		if (inited == false)
		{
			try
			{
				//�����ļ���·��
				{
					MainPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/";
					MainPath.append(pro_path.value("Parent").toString());
					if (!File.exists(MainPath))
					{
						Dir.mkdir(MainPath);
					}
					MainPath.append("/");
					MainPath.append(pro_path.value("Child").toString());
					if (!File.exists(MainPath))
					{
						Dir.mkdir(MainPath);
					}
				}
				//config.json·��
				ConfigPath = MainPath + "/config.json";
				//�û��ļ�·��
				UserFilePath = MainPath + "/user";
				//��־·��
				curDateTime = QDateTime::currentDateTime();
				LogPath = MainPath + "/log/" + curDateTime.toString("yyyy-MM-dd_hh-mm-ss") + ".log";
				file_log.setFileName(LogPath);
				Logging = true;
				if (Logging == true)
				{
					file_log.open(QIODevice::ReadWrite);
				}
				//���·��
				PackagePath = MainPath + "/package";
				//��ȡ��Ŀ�ļ�·��
				Pro_path = pro_path;
				//�����ļ�
				QFile file_config(ConfigPath);
				//���·���Ƿ����
				{
					if (!File.exists(UserFilePath))
					{
						Dir.mkdir(UserFilePath);
					}
					if (!File.exists(LogPath))
					{
						Dir.mkdir(LogPath);
					}
					if (!File.exists(PackagePath))
					{
						Dir.mkdir(PackagePath);
					}
				}

				/**�����û����ݿ�**/
				{
					if (QSqlDatabase::contains("Users"))
					{
						db = QSqlDatabase::database("Users");
					}
					else
					{
						db = QSqlDatabase::addDatabase("QSQLITE", "Users");
						db.setDatabaseName(UserFilePath + "/Users.lsf");
					}
					//�����ݿ�
					db.open();

					if (!db.isOpen())
					{
						//δ���û����ݿ�
						inited_result = -1;
						return -1;
					}
					QSqlQuery sql_query(db);
					if (!sql_query.exec("create table if not exists Users( Name VARCHAR(10), UUID VARCHAR(56) PRIMARY KEY, Password VARCHAR(64));"))
					{
						//qDebug() << "Error: Fail to create table." << sql_query.lastError();
						db.close();
						inited_result = -3;
						return -3;
					}
				}

				//��config�ļ�������
				{
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
							//δ�ɹ���config.json
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
			}
			catch (const std::exception&)
			{
				//��������
				inited_result = -2;
				return -2;
			}

		}
		//��ʼ���ɹ�/�ѳ�ʼ��
		inited_result = 0;
		inited = true;
		return 0;
	}
	/*
	init(QJsonObject project_path)
	�����ʼ��
	{
	//�����ļ���·��
	QString MainPath;

	//config.json·��
	QString ConfigPath;

	//�û��ļ�·��
	QString UserFilePath;

	//��־·��
	QString LogPath;

	//���·��
	QString PackagePath;

	//config�ļ�
	QFile file_config;

	//��Ŀ·����
	QJsonObject Pro_path;
	}
	**ע�⣺�����ǿ��ܻ��ʼ���ģ����������ֵ
	{
	//��Ŀ��ʼ����
	QJsonObject Settings;

	//��Ŀ��ǰ����
	QJsonObject Current_settings;

	}
	*/
	//��Ŀ����
	int init_settings(QJsonObject settings, bool confirm = false)
	{
		/*
		[����ֵ˵��]
		1.<0>
		��������
		2.<1>
		��ʼ����û��д��
		3.<-1>
		δ�ɹ�д��
		*/
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
	QString get_setting(QString key)
	{
		if (inited_setting=true)
		{
			return Current_settings.value(key).toString();
		}
		else
		{
			return "";
		}
	}
	//�����û�
	int reg(std::string Username, std::string Psw = "")
	{
		/*
		[����ֵ˵��]
		1.<0>:��������
		2.<-1>:δ�����ݿ�
		3.<-2>:δ��ʼ��
		4.<-3>:�������ݳ���
		5.<-4>:δ�ɹ������û��������
		6.<-5>:��ע��
		*/
		if (inited == false)
		{
			return -2;
		}
		else
		{
			get_users();
			QString username = QString::fromLocal8Bit(Username.c_str());
			if (user_list.contains(username))
			{
				return -5;
			}
			if (QSqlDatabase::contains("Users"))
			{
				db = QSqlDatabase::database("Users");
			}	
			else
			{
				db = QSqlDatabase::addDatabase("QSQLITE","Users");
				db.setDatabaseName(UserFilePath + "/Users.lsf");
			}
			//�����ݿ�
			db.open();

			if (!db.isOpen())
			{
				//δ���û����ݿ�
				return -1;
			}
			QSqlQuery sql_query(db);			
			QString psw = QString::fromLocal8Bit(Psw.c_str());
			QString cmd = "insert into Users values(?,?,?)";
			QString uuid = (QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex();
			sql_query.prepare(cmd);
			sql_query.addBindValue(username);
			sql_query.addBindValue(uuid);
			sql_query.addBindValue((QString)QCryptographicHash::hash(psw.toLatin1(), QCryptographicHash::Sha3_256).toHex());

			if (!sql_query.exec())
			{
				//qDebug() << sql_query.lastError() << endl;
				db.close();
				return -3;
			}
			if (!sql_query.exec("create table if not exists "+ uuid +"( GroupName VARCHAR PRIMARY KEY);"))
			{
				//qDebug() << "Error: Fail to create table." << sql_query.lastError();
				db.close();
				return -4;
			}
			db.close();
			return 0;
		}
	}
	//��¼ 
	int login(std::string Username, std::string Psw = "")
	{
		/*
		[����ֵ˵��]
		1.<0>:��������
		2.<-1>:δ�����ݿ�
		3.<-2>:δ��ʼ��
		4.<-3>:δ��¼�ɹ�(������û�������)
		5.<-4>:δ�ɹ������ݿ�����
		*/
		//�ж��Ƿ��ʼ��
		if (inited == false)
		{
			return -2;
		}
		//�ж��Ƿ����Users����
		if (QSqlDatabase::contains("Users"))
		{
			db = QSqlDatabase::database("Users");
		}
		else
		{
			db = QSqlDatabase::addDatabase("QSQLITE", "Users");
			db.setDatabaseName(UserFilePath + "/Users.lsf");
		}
		//�����ݿ�
		db.open();
		if (!db.isOpen())
		{
			//δ���û����ݿ�����
			return -1;
		}
		QSqlQuery sql_query(db);
		QString psw = QString::fromLocal8Bit(Psw.c_str());
		QString username = QString::fromLocal8Bit(Username.c_str());
		QString cmd = "select * from Users where Name='" + username + "' and UUID='" + (QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex() + "' and Password='" + (QString)QCryptographicHash::hash(psw.toLatin1(), QCryptographicHash::Sha3_256).toHex() + "'";
		sql_query.prepare(cmd);
		if (!sql_query.exec())
		{
			//qDebug() << "Error: Fail to select." << sql_query.lastError();
			db.close();
			return -4;
		}
		if (!sql_query.next())
		{
			db.close();
			return -3;
		}
		db.close();
		return 0;
	}
	//�鿴�����û� *[����QJsonObject user_list]
	QJsonObject get_users(void)
	{
		QJsonObject get;
		if (inited == false)
		{
			return get;
		}
		//�ж��Ƿ����Users����
		if (QSqlDatabase::contains("Users"))
		{
			db = QSqlDatabase::database("Users");
		}
		else
		{
			db = QSqlDatabase::addDatabase("QSQLITE", "Users");
			db.setDatabaseName(UserFilePath + "/Users.lsf");
		}
		//�����ݿ�
		db.open();
		if (!db.isOpen())
		{
			//δ���û����ݿ�����
			return get;
		}
		QJsonObject psw_list;
		QSqlQuery query(db);
		query.exec("select * from Users");
		QSqlRecord rec = query.record();
		QString name;
		QString uuid;
		QString psw;
		while (query.next())
		{
			rec = query.record();
			uuid = query.value(rec.indexOf("UUID")).toString();
			name = query.value(rec.indexOf("Name")).toString();
			psw = query.value(rec.indexOf("Password")).toString();
			//qDebug() << "UserName:" << name;
			//qDebug() << "UUID:" << uuid;
			get.insert(name,uuid);
			psw_list.insert(uuid, psw);
		}
		db.close();
		user_list = get;
		user_psw = psw_list;
		return get;
	}
	//�޸�����
	int chg_psw(std::string Username, std::string Psw="", std::string NewPsw = "")
	{
		/*
		[����ֵ˵��]
		1.<0>:��������
		2.<-1>:δ�ɹ����û����ݿ�
		3.<-2>:δ��ʼ��
		4.<-3>:δ�ɹ��޸�
		5.<-4>:�����ڸ��û�
		6.<-5>:�������
		*/
		if (inited == false)
		{
			return -2;
		}
		//�����û��б�
		get_users();
		QString username = QString::fromLocal8Bit(Username.c_str());
		if (!user_list.contains(username))
		{
			return -4;
		}
		if (!user_psw.contains(user_list.value(username).toString()))
		{
			return -5;
		}
		//�ж��Ƿ����Users����
		if (QSqlDatabase::contains("Users"))
		{
			db = QSqlDatabase::database("Users");
		}
		else
		{
			db = QSqlDatabase::addDatabase("QSQLITE", "Users");
			db.setDatabaseName(UserFilePath + "/Users.lsf");
		}
		//�����ݿ�
		db.open();
		if (!db.isOpen())
		{
			//δ���û����ݿ�����
			return -1;
		}
		QSqlQuery query(db);
		QString psw = QString::fromLocal8Bit(Psw.c_str());
		QString Npsw = QString::fromLocal8Bit(NewPsw.c_str());
		if (!query.exec("Update Users set Password='"+ (QString)QCryptographicHash::hash(Npsw.toLatin1(), QCryptographicHash::Sha3_256).toHex() +"' where Name='" + username + "' and UUID='" + (QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex() + "' and Password='" + (QString)QCryptographicHash::hash(psw.toLatin1(), QCryptographicHash::Sha3_256).toHex() + "'"))
		{
			return -3;
		}
		return 0;
	}
	//ɾ���û�
	int delete_user(std::string Username, std::string Psw = "")
	{
		/*
		[����ֵ˵��]
		1.<0>:��������
		2.<-1>:δ�ɹ����û����ݿ�
		3.<-2>:δ��ʼ��
		4.<-3>:δ�ɹ�ɾ��/��������
		5.<-4>:�����ڸ��û�
		6.<-5>:�������
		*/
		if (inited == false)
		{
			return -2;
		}
		//�����û��б�
		get_users();
		QString username = QString::fromLocal8Bit(Username.c_str());
		if (!user_list.contains(username))
		{
			return -4;
		}
		if (!user_psw.contains(user_list.value(username).toString()))
		{
			return -5;
		}
		//�ж��Ƿ����Users����
		if (QSqlDatabase::contains("Users"))
		{
			db = QSqlDatabase::database("Users");
		}
		else
		{
			db = QSqlDatabase::addDatabase("QSQLITE", "Users");
			db.setDatabaseName(UserFilePath + "/Users.lsf");
		}
		//�����ݿ�
		db.open();
		if (!db.isOpen())
		{
			//δ���û����ݿ�����
			return -1;
		}
		QSqlQuery query(db);
		QString psw = QString::fromLocal8Bit(Psw.c_str());

		if (query.exec("delete from Users where Name='" + username + "' and UUID='" + user_list.value(username).toString() + "' and Password='" + (QString)QCryptographicHash::hash(psw.toLatin1(), QCryptographicHash::Sha3_256).toHex() + "'"))
		{
			get_usergroups(Username);
			int i = 0;
			int len = usergroup_list.length();
			QString groupname;
			for (i; i < len; ++i)
			{
				groupname = usergroup_list.value(QString::number(i)).toString();
				_remove_user(Username, groupname.toStdString());		
			}
			//�ж��Ƿ����Users����
			if (QSqlDatabase::contains("Users"))
			{
				db = QSqlDatabase::database("Users");
			}
			else
			{
				db = QSqlDatabase::addDatabase("QSQLITE", "Users");
				db.setDatabaseName(UserFilePath + "/Users.lsf");
			}
			db.open();
			if (!query.exec("drop table " + (QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex() +" ;"))
			{
				db.close();
				//δ�ɹ�ɾ��
				return -3;
			}
			
			get_users();
			return 0;
		}
		db.close();
		return -3;
	}
	//�����û���
	int create_usergroup(std::string GroupName)
	{
		/*
		[����ֵ˵��]
		1.<0>:��������
		2.<-1>:δ�ɹ������ݿ�
		3.<-2>:δ��ʼ��
		4.<-3>:δ�ɹ������û���
		*/
		if (inited == false)
		{
			return -2;
		}
		//�ж��Ƿ����UserGroup����
		if (QSqlDatabase::contains("UserGroup"))
		{
			db = QSqlDatabase::database("UserGroup");
		}
		else
		{
			db = QSqlDatabase::addDatabase("QSQLITE", "UserGroup");
			db.setDatabaseName(UserFilePath + "/UserGroup.lsf");
		}
		//�����ݿ�
		db.open();
		if (!db.isOpen())
		{
			//δ���û����ݿ�����
			return -1;
		}
		QString groupname = QString::fromLocal8Bit(GroupName.c_str());
		QSqlQuery sql_query(db);
		if (!sql_query.exec("create table if not exists "+groupname+"( Name VARCHAR(10), UUID VARCHAR(56) PRIMARY KEY);"))
		{
			//qDebug() << "Error: Fail to create table." << sql_query.lastError();
			db.close();
			return -3;
		}
		db.close();
		return 0;
	}
	//���û���������û�
	int add_user(std::string Username, std::string GroupName)
	{
		/*
		[����ֵ˵��]
		1.<0>:��������
		2.<-1>:δ�ɹ������ݿ�
		3.<-2>:δ��ʼ��
		4.<-3>:�����ڸ��û�
		5.<-4>:δ�ɹ�����û�
		6.<-5>:��ע�� 
		7.<-6>:δ�ɹ����û�������������
		*/
		if (inited == false)
		{
			return -2;
		}
		//�����û��б�
		get_users();
		QString username = QString::fromLocal8Bit(Username.c_str());		
		if (!user_list.contains(username))
		{
			return -3;
		}
		QString groupname = QString::fromLocal8Bit(GroupName.c_str());
		//���������û��б�
		get_groupusers(GroupName);
		if (groupuser_list.contains(username))
		{
			return -5;
		}
		//�ж��Ƿ����UserGroup����
		if (QSqlDatabase::contains("UserGroup"))
		{
			db = QSqlDatabase::database("UserGroup");
		}
		else
		{
			db = QSqlDatabase::addDatabase("QSQLITE", "UserGroup");
			db.setDatabaseName(UserFilePath + "/UserGroup.lsf");
		}
		//�����ݿ�
		db.open();
		if (!db.isOpen())
		{
			//δ���û����ݿ�����
			return -1;
		}
		QSqlQuery sql_query(db);
		if (!sql_query.exec("insert into " + groupname + " values('" + username + "','" + user_list.value(username).toString() + "')"))
		{
			//qDebug() << sql_query.lastError() << endl;
			db.close();
			return -4;
		}
		db.close();
		/************����û����¼************/
		if (QSqlDatabase::contains("Users"))
		{
			db = QSqlDatabase::database("Users");
		}
		else
		{
			db = QSqlDatabase::addDatabase("QSQLITE", "Users");
			db.setDatabaseName(UserFilePath + "/Users.lsf");
		}
		//�����ݿ�
		db.open();

		if (!db.isOpen())
		{
			//δ���û����ݿ�
			return -1;
		}
		QSqlQuery query(db);
		if (!query.exec("insert into "+ user_list.value(username).toString() + " values('" + groupname + "')"))
		{
			//qDebug() << query.lastError() << endl;
			db.close();
			return -6;
		}
		
		return 0;
	}
	//������ɾ���û�
	int remove_user(std::string Username, std::string GroupName)
	{
		/*
		[����ֵ˵��]
		1.<0>:��������
		2.<-1>:δ�ɹ������ݿ�
		3.<-2>:δ��ʼ��
		4.<-3>:�����ڸ��û�
		5.<-4>:δ�ɹ�ɾ�����û�
		6.<-5>:δ�ɹ�ɾ����¼
		*/
		if (inited == false)
		{
			return -2;
		}
		//�����û��б�
		get_groupusers(GroupName);
		QString groupname = QString::fromLocal8Bit(GroupName.c_str());
		QString username = QString::fromLocal8Bit(Username.c_str());
		if (groupuser_list.contains(username)==false)
		{
			return -3;
		}
		//�ж��Ƿ����UserGroup����
		if (QSqlDatabase::contains("UserGroup"))
		{
			db = QSqlDatabase::database("UserGroup");
		}
		else
		{
			db = QSqlDatabase::addDatabase("QSQLITE", "UserGroup");
			db.setDatabaseName(UserFilePath + "/UserGroup.lsf");
		}
		//�����ݿ�
		db.open();
		if (!db.isOpen())
		{
			//δ���û����ݿ�����
			return -1;
		}
		QSqlQuery sql_query(db);
		if (!sql_query.exec("delete from "+groupname+" where Name='" + username + "' and UUID='" + groupuser_list.value(username).toString() + "'"))
		{
			//qDebug() << sql_query.lastError() << endl;
			db.close();
			return -4;
		}
		db.close();
		get_users();
		if (user_list.contains(username) == false)
		{
			return 0;
		}
		/************ɾ���û����¼************/
		{
			if (QSqlDatabase::contains("Users"))
			{
				db = QSqlDatabase::database("Users");
			}
			else
			{
				db = QSqlDatabase::addDatabase("QSQLITE", "Users");
				db.setDatabaseName(UserFilePath + "/Users.lsf");
			}
			//�����ݿ�
			db.open();
			if (!db.isOpen())
			{
				//δ���û����ݿ�
				return -1;
			}
			QSqlQuery query(db);
			if (!query.exec("delete from " + user_list.value(username).toString() + " where GroupName='" + groupname + "'"))
			{
				//qDebug() << query.lastError() << endl;
				db.close();
				return -5;
			}
		}
		return 0;
	}
	//�鿴�����û� *[����QJsonObject groupuser_list]
	QJsonObject get_groupusers(std::string GroupName)
	{
		QJsonObject get;
		if (inited == false)
		{
			return get;
		}
		//�ж��Ƿ����Users����
		if (QSqlDatabase::contains("UserGroup"))
		{
			db = QSqlDatabase::database("UserGroup");
		}
		else
		{
			db = QSqlDatabase::addDatabase("QSQLITE", "UserGroup");
			db.setDatabaseName(UserFilePath + "/UserGroup.lsf");
		}
		//�����ݿ�
		db.open();
		if (!db.isOpen())
		{
			//δ���û����ݿ�����
			return get;
		}
		QJsonObject psw_list;
		QSqlQuery query(db);
		QString groupname = QString::fromLocal8Bit(GroupName.c_str());
		if (!query.exec("select * from " + groupname))
		{
			//qDebug() << query.lastError();
			return get;
		}
		
		QSqlRecord rec = query.record();
		QString name;
		QString uuid;
		while (query.next())
		{
			rec = query.record();
			uuid = query.value(rec.indexOf("UUID")).toString();
			name = query.value(rec.indexOf("Name")).toString();
			//qDebug() << "UserName:" << name;
			//qDebug() << "UUID:" << uuid;
			get.insert(name, uuid);
		}
		db.close();
		groupuser_list = get;
		return get;
	}
	//�鿴�û��������û��� *[����QJsonObject groupuser_list]
	QJsonObject get_usergroups(std::string Username)
	{
		QJsonObject get;
		if (inited == false)
		{
			return get;
		}
		//�ж��Ƿ����Users����
		if (QSqlDatabase::contains("Users"))
		{
			db = QSqlDatabase::database("Users");
		}
		else
		{
			db = QSqlDatabase::addDatabase("QSQLITE", "Users");
			db.setDatabaseName(UserFilePath + "/Users.lsf");
		}
		//�����ݿ�
		db.open();
		if (!db.isOpen())
		{
			//δ���û����ݿ�����
			return get;
		}
		QSqlQuery query(db);
		QString username = QString::fromLocal8Bit(Username.c_str());
		query.exec("select * from " + user_list.value(username).toString());
		QSqlRecord rec = query.record();
		QString name;
		QString uuid;
		int i = 0;
		while (query.next())
		{
			rec = query.record();
			uuid = query.value(rec.indexOf("GroupName")).toString();
			get.insert(QString::number(i), uuid);
			++i;
		}
		db.close();
		usergroup_list = get;
		return get;
	}
	//������־
	int write_log(std::string Info,std::string Utils="Utils" , std::string Type="INFO")
	{
		/*
		[����ֵ˵��]
		1.<0>:��������
		2.<-1>:δ�ɹ�����־�ļ�
		3.<-2>:δ��ʼ��
		*/
		if (inited == false)
		{
			return -2;
		}

		if (!file_log.isOpen())
		{
			if (!file_log.open(QIODevice::Append))
			{
				return -1;
			}
		}
		QString info = QString::fromLocal8Bit(Info.c_str());
		QString utils = QString::fromLocal8Bit(Utils.c_str());
		QString type = QString::fromLocal8Bit(Type.c_str());
		QTextStream stream(&file_log);
		curDateTime = QDateTime::currentDateTime();
		stream << curDateTime.toString("yyyy-MM-dd hh:mm:ss") << " - " << utils << ": " << type << ":" << info << "\n";
		file_log.close();
		return 0;
	}

	/*��������*/
	~LinkerCore()
	{
		//�����û�����
		if (File.exists(UserFilePath + "/Users.lsf.bak"))
		{
			Dir.remove(UserFilePath + "/Users.lsf.bak");
		}
		File.copy(UserFilePath + "/Users.lsf", UserFilePath + "/Users.lsf.bak");
		//�����û�����Ϣ
		if (File.exists(UserFilePath + "/UserGroup.lsf.bak"))
		{
			Dir.remove(UserFilePath + "/UserGroup.lsf.bak");
		}
		File.copy(UserFilePath + "/UserGroup.lsf", UserFilePath + "/UserGroup.lsf.bak");
		if (file_log.isOpen())
		{
			file_log.close();
		}
	}
};