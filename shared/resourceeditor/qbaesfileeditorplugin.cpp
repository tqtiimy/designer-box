#include "qbaesfileeditorplugin.h"

#include "texteditor/qplaintexteditor.h"
#include "imageeditor/qimageeditor.h"

#include <qplugin.h>

QString QBaesFileEditorPlugin::get_plugin_name()
{
	return EDITOR_PLUGIN_BASE_NAME;
}

QAbstractResourceEditor *QBaesFileEditorPlugin::create(const QString &name)
{
	QAbstractResourceEditor *obj = NULL;

	if (name == "Plain.Text.Editor")
	{
		obj = new QPlainTextEditor;
	}
	else if (name == "Image.Editor")
	{
		obj = new QImageEditor;
	}

	return obj;
}

QStringList QBaesFileEditorPlugin::supports()
{
	QStringList list;
	list << "Plain.Text.Editor";
	list << "Image.Editor";
	return list;
}

Q_EXPORT_PLUGIN2(QBaesFileEditorPlugin, QBaesFileEditorPlugin)
