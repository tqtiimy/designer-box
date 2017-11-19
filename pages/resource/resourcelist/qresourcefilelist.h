#ifndef QRESOURCEFILELIST_H
#define QRESOURCEFILELIST_H

#include <QWidget>
#include <QUndoStack>
#include <QTreeWidgetItem>

class QFileListView;
class StyledBar;
struct tagFileInfo;
struct tagFileGroupInfo;

class QResourceFileList : public QWidget
{
	Q_OBJECT
public:
	explicit QResourceFileList(QWidget *parent = 0);

	void    set_undo_stack(QUndoStack *stack);
protected:
	bool eventFilter(QObject *, QEvent *);
signals:

protected slots:
	void    add_group();
	void    remove_group(tagFileGroupInfo *group);
	void    add_file();
	void    remove_file(tagFileInfo *file);
signals:
	void    select(tagFileInfo *f);
protected:
	QFileListView       *m_file_list_view;
	StyledBar           *m_styled_bar;
	QUndoStack          *m_undo_stack;
};

#endif // QRESOURCEFILELIST_H
