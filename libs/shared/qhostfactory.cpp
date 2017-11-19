#include "qhostfactory.h"

#include "host/qabstracthost.h"

#include "host/qformhost.h"

#include "xmlobject.h"

QMap<QString, tagHostInfo *> QHostFactory::m_meta_map;

QHostFactory::QHostFactory()
{
}

void QHostFactory::register_host(const QString name, GET_SHOW_ICON icon, GET_SHOW_NAME get_name, GET_SHOW_GROUP get_group, const QMetaObject *host)
{
	tagHostInfo *info = m_meta_map.value(name);

	if (info == NULL)
	{
		info = new tagHostInfo;
	}

	info->m_host_object = host;
	info->m_name = name;
	info->get_show_icon = icon;
	info->get_show_name = get_name;
	info->get_show_group = get_group;
	m_meta_map.insert(name, info);
}

QAbstractHost *QHostFactory::create_host(const QString &name)
{
	QAbstractHost *host = NULL;

	if (name == FORM_TITLE)
	{
		host = new QFormHost;
		host->init();
		host->set_attribute(HOST_TYPE, FORM_TITLE);
	}
	else
	{
		tagHostInfo *info = m_meta_map.value(name);

		if (info == NULL)
		{
			return NULL;
		}

		const QMetaObject *obj = info->m_host_object;

		if (obj != NULL)
		{
			host = (QAbstractHost *)obj->newInstance();
			host->init();
			host->set_attribute(HOST_TYPE, name);
		}
	}

	return host;
}

QMap<QString, tagHostInfo *> QHostFactory::get_host_info()
{
	return m_meta_map;
}

QAbstractHost *QHostFactory::create_host(XMLObject *xml)
{
	if (xml == NULL)
	{
		return NULL;
	}

	QAbstractHost *ret = NULL;

	if (xml->get_title() == FORM_TITLE)
	{
		ret = new QFormHost;
		ret->init();
		ret->from_object(xml);
		ret->set_attribute(HOST_TYPE, FORM_TITLE);
	}
	else
	{
		ret = create_host(xml->get_property(HOST_TYPE));

		if (ret != NULL)
		{
			ret->from_object(xml);
		}
	}

	return ret;
}

tagHostInfo *QHostFactory::get_host_info(const QString &name)
{
	return m_meta_map.value(name);
}
