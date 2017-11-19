#include "qhostobjectmap.h"

#include "../../qsoftcore.h"

#include "../../../shared/host/qabstracthost.h"
#include "../../../shared/qprojectcore.h"
#include "../../../shared/property/qabstractproperty.h"

#include "../../../shared/qpagemanager.h"
#include "../../../shared/qdatamanager.h"
#include "../../../shared/qdata.h"

#include <QVariant>
#include <QMetaObject>
#include <QMetaMethod>

QHostObjectMap::QHostObjectMap(QAbstractProperty *property, QAbstractHost *host)
{
	QAbstractHost *h = QSoftCore::get_core()->get_project_core()->get_project_host();

	bool keyboard = host->property("title").toString() == "keyboard";
	bool project = host->property("title").toString() == "Project";

	tagHostInfo *info = make_info(h);
	info->m_name = "global";

	QList<QData *> groups = QSoftCore::get_core()->get_project_core()->get_data_manager()->get_all_datas();

	foreach(QData * g, groups)
	{
		if (g->get_datas().size() > 0)
		{
			tagHostInfo *temp = new tagHostInfo;
			temp->m_name = g->get_name();
			foreach(tagDataInfo * data, g->get_datas())
			{
				tagHostInfo *tt = new tagHostInfo;
				tt->m_name = data->m_name;
				tt->m_type = "data";
				temp->m_children.insert(tt->m_name, tt);
			}
			temp->m_type = "data";
			info->m_children.insert(temp->m_name, temp);
		}
	}

	m_infos.insert("global", info);
	QList<QAbstractHost *>   list = QSoftCore::get_core()->get_project_core()->get_page_manager()->get_pages();

	if (!keyboard || project)
	{
		foreach(QAbstractHost * h, list)
		{
			if (h->property("title").toString() != "keyboard" || project)
			{
				tagHostInfo *info = make_info(h);
				m_infos.insert(info->m_name, info);
			}
		}
	}

	info = make_info(host);
	info->m_name = "self";
	m_infos.insert(info->m_name, info);

	if (!keyboard || project)
	{
		if (host->get_parent() != NULL)
		{
			info = make_info(host->get_parent());
			info->m_name = "parent";
			m_infos.insert(info->m_name, info);
		}
	}
	else
	{
		info = make_info(host);
		m_infos.insert(info->m_name, info);
	}

	m_param_list = property->get_attribute("param").toStringList();
}

tagHostInfo *QHostObjectMap::make_info(QAbstractHost *host)
{
	tagHostInfo *info = new tagHostInfo;
	info->m_host = host;
	info->m_type = "form";
	info->m_name = host->get_property_value("objectName").toString();

	QList<QAbstractHost *>    list = host->get_children();

	foreach(QAbstractHost * h, list)
	{
		tagHostInfo *in = make_info(h);
		info->m_children.insert(in->m_name, in);
	}

	const QMetaObject *meta = host->metaObject();

	int count = meta->methodCount();
	QString str;
	QStringList l = host->property("function_list").toStringList();

	for (int i = 0; i < count; i++)
	{
		QMetaMethod method = meta->method(i);

		if (method.methodType() == QMetaMethod::Slot && method.access() == QMetaMethod::Public)
		{
			str = method.signature();

			if (l.contains(str))
			{
				continue;
			}

			int index = str.indexOf("(");
			str = str.left(index);

			if (!info->m_children.keys().contains(str))
			{
				info->m_methods.insert(str, method);
			}
		}
	}

	return info;
}

tagHostInfo   *QHostObjectMap::get_all_by_text(const QString &text)
{
	QString s = text;

	if (s.endsWith("."))
	{
		s = s.left(s.size() - 1);
	}

	QStringList list = s.split(".");

	if (list.size() == 0)
	{
		return NULL;
	}

	tagHostInfo *info = m_infos.value(list.takeFirst());

	while (info != NULL)
	{
		if (list.size() == 0)
		{
			break;
		}

		info = info->m_children.value(list.takeFirst());
	}

	return info;
}

tagReturnList QHostObjectMap::get_info(const QString &text)
{
	tagReturnList ret;
	int index = text.lastIndexOf(".");
	QString temp1, temp2;

	if (index > 0)
	{
		tagHostInfo *info = get_all_by_text(text.left(index));

		if (info != NULL)
		{
			QString s = text.mid(index + 1);
			temp1 = s.toLower();
			QList<tagReturnInfo>    retList1, retList2;

			if (info->m_type == "form")
			{
				QMapIterator<QString, tagHostInfo *>  it(info->m_children);

				while (it.hasNext())
				{
					it.next();
					temp2 = it.key().toLower();
					tagReturnInfo info;
					info.m_type = TYPE_FORM;
					info.m_param = it.key();
					info.m_is_last = false;

					if (temp2.startsWith(temp1))
					{
						retList1.append(info);
					}
					else if (temp2.indexOf(temp1) > 0)
					{
						retList2.append(info);
					}
				}

				QMapIterator<QString, QMetaMethod>  itt(info->m_methods);

				while (itt.hasNext())
				{
					itt.next();
					temp2 = itt.key().toLower();
					tagReturnInfo info;
					info.m_type = TYPE_FUNCTION;
					info.m_method = itt.value();
					info.m_param = itt.key();
					info.m_is_last = true;

					if (temp2.startsWith(temp1))
					{
						retList1.append(info);
					}
					else if (temp2.indexOf(temp1) > 0)
					{
						retList2.append(info);
					}
				}
			}
			else
			{
				QMapIterator<QString, tagHostInfo *>  it(info->m_children);

				while (it.hasNext())
				{
					it.next();
					temp2 = it.key().toLower();
					tagReturnInfo info;
					info.m_type = TYPE_DATA;
					info.m_param = it.key();
					info.m_is_last = it.value()->m_children.size() == 0;

					if (temp2.startsWith(temp1))
					{
						retList1.append(info);
					}
					else if (temp2.indexOf(temp1) > 0)
					{
						retList2.append(info);
					}
				}
			}

			ret.m_returns = retList1 + retList2;
		}
	}
	else
	{
		QList<tagReturnInfo>    retList1, retList2;
		QMapIterator<QString, tagHostInfo *>  it(m_infos);
		temp1 = text.toLower();

		while (it.hasNext())
		{
			it.next();
			temp2 = it.key().toLower();
			tagReturnInfo info;
			info.m_type = TYPE_FORM;
			info.m_param = it.key();
			info.m_is_last = false;

			if (temp2.startsWith(temp1))
			{
				retList1.append(info);
			}
			else if (temp2.indexOf(temp1) > 0)
			{
				retList2.append(info);
			}
		}

		foreach(const QString & str, m_param_list)
		{
			temp2 = str.toLower();
			tagReturnInfo info;
			info.m_type = TYPE_PARAM;
			info.m_param = str;
			info.m_is_last = true;

			if (temp2.startsWith(temp1))
			{
				retList1.append(info);
			}
			else if (temp2.indexOf(temp1) > 0)
			{
				retList2.append(info);
			}
		}
		ret.m_returns = retList1 + retList2;
	}

	foreach(const tagReturnInfo & info, ret.m_returns)
	{
		ret.m_name_to_return.insert(info.m_param, info);
	}

	return ret;
}
