#include "LinkerCore.h"
#include "LinkerProject.h"




int main(int argc, char* argv[])
{
	LinkerCore LC;
	LinkerProject::Linker Linker_pro;
	Linker_pro.init();
	LC.init_settings(Linker_pro.Settings);
	qDebug() << QString::fromLocal8Bit("��ʼ��:") << LC.init(Linker_pro.MainPath) << endl;
	/*qDebug() << QString::fromLocal8Bit("���Ի�ȡ����:") << LC.get_setting("Project") << endl;
	qDebug() << QString::fromLocal8Bit("����ע��:" )<< LC.reg("����") << endl;
	qDebug() << QString::fromLocal8Bit("���Ե�¼:") << LC.login("����") << endl;
	qDebug() << QString::fromLocal8Bit("���Դ����û���'��ͨ':") << LC.create_usergroup("��ͨ") << endl;
	qDebug() << QString::fromLocal8Bit("��������û����û���'��ͨ':") << LC.add_user("����", "��ͨ") << endl;
	qDebug() << LC.get_groupusers("��ͨ");
	qDebug() << LC.get_usergroups("����") << endl;
	qDebug() << QString::fromLocal8Bit("����ɾ���û�'����':") << LC.delete_user("����") << endl;
	qDebug() << QString::fromLocal8Bit("��������ɾ���û�'����':") << LC.remove_user("����","��ͨ") << endl;*/


	LC.write_log("��ʼ��:" + QString::number(LC.inited_result).toStdString());
	LC.write_log("����ע��:" + QString::number(LC.reg("����")).toStdString());
	LC.write_log("���Ե�¼:" + QString::number(LC.login("����")).toStdString());
	LC.write_log("���Դ����û���'��ͨ':"+ QString::number(LC.create_usergroup("��ͨ")).toStdString());
	LC.write_log("��������û����û���'��ͨ':" + QString::number(LC.add_user("����", "��ͨ")).toStdString());
	qDebug() << LC.get_groupusers("��ͨ");
	qDebug() << LC.get_usergroups("����") << endl;
	LC.write_log("����ɾ���û�'����':" + QString::number(LC.delete_user("����")).toStdString());
	LC.write_log("��������ɾ���û�'����':" + QString::number(LC.remove_user("����", "��ͨ")).toStdString());

	/*
	 LC.init(Linker_pro.MainPath)
	 LC.reg("����")
	 LC.login("����")
	 LC.create_usergroup("��ͨ")
	 LC.add_user("����", "��ͨ")
	 LC.delete_user("����")
	 LC.remove_user("����","��ͨ")
	*/
} 

