#include <QtGui/QApplication>

#include <QFile>
#include <QProcess>
#include <QSettings>
#include <QDir>
#include <QStringList>
#include <QTimer>

#include <windows.h>

struct tagFileInfo
{
	QString m_name;
	int     m_type;
	char   *m_data;
	int     m_size;
};

QString applicationPath;


bool update()
{
	QString path = applicationPath + "/temp.tmp";

	QFile file(path);
	QList<tagFileInfo *>     list;

	if (file.open(QFile::ReadWrite))
	{
		QByteArray array = file.readAll();
		file.close();
		file.remove();

		if (array.size() <= 0)
		{
			goto LOOP;
		}

		char *data = array.data();
		int pt = 0;
		int size = array.size();

		while (pt < size)
		{
			tagFileInfo *info = new tagFileInfo;
			list.append(info);
			info->m_name = QString(data + pt);
			pt += (strlen(data + pt) + 1);

			if (pt >= size)
			{
				goto LOOP;
			}

			memcpy((char *)&info->m_type, data + pt, 4);
			pt += 4;

			if (pt >= size)
			{
				goto LOOP;
			}

			memcpy((char *)&info->m_size, data + pt, 4);
			pt += 4;

			if (pt >= size)
			{
				goto LOOP;
			}

			info->m_data = data + pt;
			pt += info->m_size;

			if (pt > size)
			{
				goto LOOP;
			}
		}

		foreach(tagFileInfo * info, list)
		{
			if (info->m_type == 1 || info->m_type == 2)
			{
				QString str = applicationPath + "/" + info->m_name;
				str.replace("\\", "/");
				int index = str.lastIndexOf("/");
				QDir dir;
				dir.mkpath(str.left(index));
				QFile f(str);

				if (f.open(QFile::WriteOnly))
				{
					f.resize(0);
					f.write(info->m_data, info->m_size);
					f.close();
				}
			}
			else
			{
				QSettings s(applicationPath + "/config.ini", QSettings::IniFormat);
				QString proPath = s.value("project/path").toString();

				if (proPath == "")
				{
					proPath = applicationPath + "/project";
				}

				QString str = proPath + "/" + info->m_name;
				str.replace("\\", "/");
				int index = str.lastIndexOf("/");
				QDir dir;
				dir.mkpath(str.left(index));
				QFile f(str);

				if (f.open(QFile::WriteOnly))
				{
					f.resize(0);
					f.write(info->m_data, info->m_size);
					f.close();
				}
			}
		}
		return true;
	}

LOOP:
	qDeleteAll(list);
	return false;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	applicationPath = a.applicationDirPath();
	bool ret = update();

	QStringList list;
	list << QString("update=%1").arg(ret ? "true" : "false");

	QProcess::startDetached(applicationPath + "/QDeviceSoft.exe", list);
	return 0;
}
