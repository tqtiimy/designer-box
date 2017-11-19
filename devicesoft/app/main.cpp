#include <QtGui/QApplication>

#include "qprojectloader.h"

#include "qdeviceapplication.h"

#include "../../libs/shared/pluginloader.h"
#include "../../libs/running/qrunningmanager.h"
#include "../../libs/shared/qbaseinit.h"

#include <QFont>
#include <QMessageBox>
#include <QTextCodec>
#include <QSettings>

int main(int argc, char *argv[])
{
	QDeviceApplication a(argc, argv);


	QTextCodec *gbk = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForTr(gbk);
	QTextCodec::setCodecForLocale(gbk);
	QTextCodec::setCodecForCStrings(gbk);

	QFont f = a.font();
	f.setFamily("Arial");
	a.setFont(f);

	qRegisterBaseProperty();
	qRegisterBaseHost();
	qRegisterBaseDriver();

	PluginLoader::load_plugin(a.applicationDirPath() + "/plugins.xml");

	QProjectLoader l;
	l.run();

	return a.exec();
}
