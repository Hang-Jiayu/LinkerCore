#pragma once

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



class LinkerCore
{
private:
	//�Ƿ��ʼ��
	bool inited = false;
	//�Ƿ��¼
	bool logined = false;
	//�Ƿ��ʼ������
	bool inited_setting = false;
	//��ʼ�����
	int inited_result = -3;
	//�Ƿ�ʹ������
	bool use_pw;


public:
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

	//�ɺ��� show_users ��ֵ
	QJsonObject user_list;

	//��ʼ����
	int init(QJsonObject pro_path)
	{
		/*
		[����ֵ˵��]
		1.<0>:��������
		2.<-1>:δ�ɹ���config.json
		3.<-2>:δ֪����
		*/
		if (inited == false)
		{

			try
			{
				//�����ļ���·��
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
				//config.json·��
				ConfigPath = MainPath + "/config.json";
				//�û��ļ�·��
				UserFilePath = MainPath + "/user";
				//��־·��
				LogPath = MainPath + "/log";
				//���·��
				PackagePath = MainPath + "/package";
				//��ȡ��Ŀ�ļ�·��
				Pro_path = pro_path;
				//�����ļ�
				QFile file_config(ConfigPath);
				
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
		4.<-3>:�������ݳ���/��ע��
		*/
		if (inited == false)
		{
			return -2;
		}
		else
		{

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
			QString username = QString::fromLocal8Bit(Username.c_str());

			QString cmd;


			if (psw == "" && username != "")
			{
				cmd = "create table if not exists Users ( Name VARCHAR(10), UUID VARCHAR(56), PRIMARY KEY(UUID))";
				sql_query.prepare(cmd);
				if (!sql_query.exec())
				{
					//qDebug() << "Error: Fail to create table." << sql_query.lastError();
					db.close();
					return -2;
				}
				cmd = "insert into Users values(?,?)";
				sql_query.prepare(cmd);
				sql_query.addBindValue(username);
				sql_query.addBindValue((QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex());
			}
			else if (psw != "" && username != "")
			{
				cmd = "create table if not exists Users ( Name VARCHAR(10), UUID VARCHAR(56), Password VARCHAR(64), PRIMARY KEY(UUID))";
				sql_query.prepare(cmd);
				if (!sql_query.exec())
				{
					//qDebug() << "Error: Fail to create table." << sql_query.lastError();
					db.close();
					return -2;
				}
				cmd = "insert into Users values(?,?,?)";
				sql_query.prepare(cmd);
				sql_query.addBindValue(username);
				sql_query.addBindValue((QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex());
				sql_query.addBindValue((QString)QCryptographicHash::hash(psw.toLatin1(), QCryptographicHash::Sha3_256).toHex());
			}
			if (!sql_query.exec())
			{
				//qDebug() << sql_query.lastError() << endl;
				db.close();
				return -3;
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
		if (psw == "" && username != "")
		{
			QString cmd = "select * from Users where Name='"+ username +"' and UUID='"+ (QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex()+"'";
			sql_query.prepare(cmd);
			if (!sql_query.exec())
			{
				//qDebug() << "Error: Fail to select." << sql_query.lastError();
				db.close();
				return -2;
			}
			if (!sql_query.next())
			{
				db.close();
				return -3;
			}
			db.close();
			return 0;
		}
		else if (psw != "" && username != "")
		{
			QString cmd = "select * from Users where Name='" + username + "' and UUID='" + (QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex() + "' and Password='" + (QString)QCryptographicHash::hash(psw.toLatin1(), QCryptographicHash::Sha3_256).toHex() + "'";
			sql_query.prepare(cmd);
			if (!sql_query.exec())
			{
				//qDebug() << "Error: Fail to select." << sql_query.lastError();
				db.close();
				return -2;
			}
			if (!sql_query.next())
			{
				db.close();
				return -3;
			}
			db.close();
			return 0;
		}
		db.close();
		return 0;
	


	}
	//�鿴�����û� *[����QJsonObject user_list]
	QJsonObject show_users(void)
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
		query.exec("select * from Users");
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
			get.insert(name,uuid);

		}
		db.close();
		user_list = get;
		return get;
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
		*/
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
		QSqlQuery query(db);
		QString username = QString::fromLocal8Bit(Username.c_str());
		QString psw = QString::fromLocal8Bit(Psw.c_str());
		if (Username != "" && psw == "")
		{
			if (query.exec("delete from Users where Name='" + username + "' and UUID='" + (QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex() + "'"))
			{
				db.close();
				show_users();
				if (user_list.value(username) == (QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex())
				{
					//δ�ɹ�ɾ��
					return -3;
				}
				return 0;
			}
		}
		else if (Username != "" && psw != "")
		{
			if (query.exec("delete from Users where Name='" + username + "' and UUID='" + (QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex() + "' and Password='" + (QString)QCryptographicHash::hash(psw.toLatin1(), QCryptographicHash::Sha3_256).toHex() + "'"))
			{
				db.close();
				show_users();
				if (user_list.value(username) == (QString)QCryptographicHash::hash(username.toLatin1(), QCryptographicHash::Sha3_224).toHex())
				{
					//δ�ɹ�ɾ��
					return -3;
				}
				return 0;
			}
		}
		db.close();
		return -3;
	}


};