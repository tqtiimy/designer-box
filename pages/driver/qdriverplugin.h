#ifndef QDRIVERPLUGIN_H
#define QDRIVERPLUGIN_H

#include "../../libs/core/qabstractpage.h"

#define DRIVER_PLUGIN_NAME "Plugin.Page.Driver"

class QDriverPlugin : public QAbstractPage
{
	Q_OBJECT
public:
	QString     get_plugin_name();//插件名称
	QString     get_page_name();//获取页面的名称
	QIcon       get_page_icon();//获取页面的图标
	int         get_index();//获取页面索引，用于在标签中的位置
	bool        get_enabled();//工程关闭的时候是否启用这个页面
protected:
	void        create_widget();
};

#endif // QDRIVERPLUGIN_H
