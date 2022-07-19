#include "LinkerCore.h"
#include "LinkerProject.h"




int main(int argc, char* argv[])
{
	LinkerCore LC;
	LinkerProject::Linker Linker_pro;
	Linker_pro.init();
	LC.init_settings(Linker_pro.Settings);
	qDebug() << QString::fromLocal8Bit("��ʼ��:") << LC.init(Linker_pro.MainPath) << endl;
	qDebug() << QString::fromLocal8Bit("���Ի�ȡ����:") << LC.get_setting("Project") << endl;
	qDebug() << QString::fromLocal8Bit("����ע��:" )<< LC.reg("�����˺�") << endl;
	qDebug() << QString::fromLocal8Bit("���Ե�¼:") << LC.login("�����˺�") << endl;
} 

