#include "qfontproperty.h"

#include "qintproperty.h"
#include "qboolproperty.h"
#include "qenumproperty.h"

#include "../qcommonstruct.h"

#include <QFontDatabase>
#include <QApplication>
#include <QFont>
#include <QImage>
#include <QPainter>
static QFontDatabase    fontDatabase;

QFontProperty::QFontProperty(QAbstractProperty *parent):
	QAbstractProperty(parent)
{
	set_property("type", "Font");

	m_family = new QEnumProperty(this);
	m_family->set_attribute("show_name", tr("Family"));
	m_family->set_property("name", "Family");
	QStringList list = fontDatabase.families();
	tagComboItem item;
	ComboItems  items;
	foreach(QString str, list)
	{
		item.m_text = str;
		item.m_value = str;
		items.append(item);
	}
	QVariant v;
	v.setValue<ComboItems>(items);
	m_family->set_attribute("items", v);


	m_pointSize = new QIntProperty(this);
	m_pointSize->set_attribute("show_name", tr("Size"));
	m_pointSize->set_property("name", "Size");

	m_bold = new QBoolProperty(this);
	m_bold->set_attribute("show_name", tr("Bold"));
	m_bold->set_property("name", "Bold");


	m_italic = new QBoolProperty(this);
	m_italic->set_attribute("show_name", tr("Italic"));
	m_italic->set_property("name", "Italic");

	m_underline = new QBoolProperty(this);
	m_underline->set_attribute("show_name", tr("Underline"));
	m_underline->set_property("name", "Underline");

	m_strikeout = new QBoolProperty(this);
	m_strikeout->set_attribute("show_name", tr("Strikeout"));
	m_strikeout->set_property("name", "Strikeout");
}

void QFontProperty::set_value(const QVariant &value)
{
	if (!value.isValid())
	{
		return;
	}

	disconnect_children();

	QFont f = value.value<QFont>();
	m_family->set_value(f.family());
	m_bold->set_value(f.bold());
	m_pointSize->set_value(f.pointSize());
	m_italic->set_value(f.italic());
	m_underline->set_value(f.underline());
	m_strikeout->set_value(f.strikeOut());
	QAbstractProperty::set_value(value);

	connect_children();
}

QString QFontProperty::get_value_text()
{

	QFont f = get_value().value<QFont>();
	return f.family();
}

QIcon QFontProperty::get_value_icon()
{
	QFont f = get_value().value<QFont>();
	QImage img(16, 16, QImage::Format_ARGB32_Premultiplied);
	img.fill(0);
	QPainter p(&img);
	p.setRenderHint(QPainter::TextAntialiasing, true);
	p.setRenderHint(QPainter::Antialiasing, true);
	f.setPointSize(13);
	p.setFont(f);
	QTextOption t;
	t.setAlignment(Qt::AlignCenter);
	p.drawText(QRect(0, 0, 16, 16), QString(QLatin1Char('A')), t);
	return QPixmap::fromImage(img);
}

void QFontProperty::child_value_changed(const QVariant &, const QVariant &)
{
	if (m_pointSize->get_value().toInt() <= 0)
	{
		return;
	}

	QFont f = get_value().value<QFont>();
	f.setBold(m_bold->get_value().toBool());
	f.setPointSize(m_pointSize->get_value().toInt());
	f.setItalic(m_italic->get_value().toBool());
	f.setUnderline(m_underline->get_value().toBool());
	f.setStrikeOut(m_strikeout->get_value().toBool());
	f.setFamily(m_family->get_value().toString());
	QVariant v;
	v.setValue<QFont>(f);
	QAbstractProperty::set_value(v);
}

void QFontProperty::make_value()
{
	QFont f = get_value().value<QFont>();
	f.setBold(m_bold->get_value().toBool());
	f.setPointSize(m_pointSize->get_value().toInt());
	f.setItalic(m_italic->get_value().toBool());
	f.setUnderline(m_underline->get_value().toBool());
	f.setStrikeOut(m_strikeout->get_value().toBool());
	f.setFamily(m_family->get_value().toString());
	m_value.setValue<QFont>(f);
}

void QFontProperty::write_value()
{

}
