#include <QApplication>

#include "mainwindow.h"
#include "qdebugwidget.h"

#include "../libs/shared/pluginloader.h"
#include "../libs/shared/qbaseinit.h"
#include "../libs/core/qbaseinit.h"
#include "../libs/core/qsoftcore.h"

#include <QFont>
#include <QTextCodec>

#include <QUuid>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTextCodec *gbk = QTextCodec::codecForName("GBK");
	QTextCodec::setCodecForTr(gbk);
	QTextCodec::setCodecForLocale(gbk);
	QTextCodec::setCodecForCStrings(gbk);

	QFont f = a.font();
	f.setFamily("Arial");
	f.setPointSize(9);
	a.setFont(f);

	qRegisterBaseProperty();
	qRegisterBasePropertyEditor();
	qRegisterBaseHost();
	qRegisterBaseDriver();

	PluginLoader::load_plugin(a.applicationDirPath() + "/plugins.xml");

	QSoftCore::get_core()->start_update_plugin();

	g_debugWidget = new QDebugWidget;

	MainWindow w;
	w.show();

	int ret = a.exec();
	return ret;
}
