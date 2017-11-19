#include "qstringchangedundocommand.h"

#include "../qsoftcore.h"


#include "../../shared/qprojectcore.h"
#include "../../shared/qlanguagemanager.h"
#include "../../shared/qlanguage.h"
#include "../../shared/property/qabstractproperty.h"
#include "../../shared/host/qabstracthost.h"

QStringChangedUndoCommand::QStringChangedUndoCommand(const QString &host_uuid,
        const QString &property_name,
        const QString &uuid,
        bool old_tr,
        bool new_tr,
        const QString &old_text,
        const QString &new_text,
        const QMap<QString, QString> &old_translate,
        const QMap<QString, QString> &new_translate,
        QUndoCommand *parent):
	QBaseUndoCommand(parent),
	m_host_uuid(host_uuid),
	m_property_name(property_name),
	m_uuid(uuid),
	m_old_tr(old_tr),
	m_new_tr(new_tr),
	m_old_text(old_text),
	m_new_text(new_text),
	m_old_translates(old_translate),
	m_new_translates(new_translate)
{
}

int QStringChangedUndoCommand::id()const
{
	return STRING_CHANGED_UNDO_COMMAND;
}

void QStringChangedUndoCommand::redo()
{
	QAbstractHost *h = QSoftCore::get_core()->get_project_core()->get_host_by_uuid(m_host_uuid);

	if (h != NULL)
	{
		QAbstractProperty *pro = h->get_property(m_property_name);

		if (pro != NULL)
		{
			pro->set_property("tr", m_new_tr);
			pro->set_property("uuid", m_new_tr ? m_uuid : QVariant());
		}

		QLanguageManager *manager = QSoftCore::get_core()->get_project_core()->get_language_manager();

		if (m_old_tr)
		{
			if (!m_new_tr)
			{
				manager->remove_text(m_uuid);
				h->set_property_value(m_property_name, m_new_text);
			}
			else
			{
				QMapIterator<QString, QString> it(m_new_translates);

				while (it.hasNext())
				{
					it.next();
					manager->set_text(m_uuid, it.value(), it.key());
				}
			}
		}
		else
		{
			if (m_new_tr)
			{
				QMapIterator<QString, QString> it(m_new_translates);

				while (it.hasNext())
				{
					it.next();
					manager->add_translate(it.key(), m_uuid, it.value());
				}
			}
			else
			{
				h->set_property_value(m_property_name, m_new_text);
			}
		}
	}
}

void QStringChangedUndoCommand::undo()
{
	QAbstractHost *h = QSoftCore::get_core()->get_project_core()->get_host_by_uuid(m_host_uuid);

	if (h != NULL)
	{
		QAbstractProperty *pro = h->get_property(m_property_name);

		if (pro != NULL)
		{
			pro->set_property("tr", m_old_tr);
			pro->set_property("uuid", m_old_tr ? m_uuid : QVariant());
		}

		QLanguageManager *manager = QSoftCore::get_core()->get_project_core()->get_language_manager();

		if (m_new_tr)
		{
			if (!m_old_tr)
			{
				manager->remove_text(m_uuid);
				h->set_property_value(m_property_name, m_old_text);
			}
			else
			{
				QMapIterator<QString, QString> it(m_old_translates);

				while (it.hasNext())
				{
					it.next();
					manager->set_text(m_uuid, it.value(), it.key());
				}
			}
		}
		else
		{
			if (m_old_tr)
			{
				QMapIterator<QString, QString> it(m_old_translates);

				while (it.hasNext())
				{
					it.next();
					manager->add_translate(it.key(), m_uuid, it.value());
				}
			}
			else
			{
				h->set_property_value(m_property_name, m_old_text);
			}
		}
	}
}
