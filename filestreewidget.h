#ifndef FILESTREEWIDGET_H
#define FILESTREEWIDGET_H

#include <QDropEvent>
#include <QTreeWidget>
#include <QMimeData>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDir>


class FilesTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    FilesTreeWidget(QWidget *parent= Q_NULLPTR):
        QTreeWidget(parent)
    {
        setAcceptDrops(true);
        setDropIndicatorShown(true);
    }

signals:
    void addItem(QTreeWidgetItem *);

protected:
    bool dropMimeData(QTreeWidgetItem *parent, int /*index*/, const QMimeData *data, Qt::DropAction /*action*/) {
        for(const QUrl url: data->urls()) {
            const QFileInfo info( url.toLocalFile());
            if(info.isFile()) {
                QTreeWidgetItem *item = new QTreeWidgetItem(parent);
                if (parent){
                    parent->setExpanded(true);
                    item->setIcon(0, QIcon(":/icons/page.png"));
                    item->setText(0, info.fileName());
                    item->setData(0, Qt::UserRole, QDir::toNativeSeparators(info.filePath()));
                    item->setFlags(Qt::ItemNeverHasChildren | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    emit addItem(item);
                }
            }
        }
        return true;
    }

    QStringList mimeTypes () const
    {
        return QStringList()<<"text/uri-list";
    }

    Qt::DropActions supportedDropActions () const
    {
        return Qt::CopyAction | Qt::MoveAction;
    }

};

#endif // FILESTREEWIDGET_H
