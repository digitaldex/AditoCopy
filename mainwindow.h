#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTreeWidgetItemIterator>
#include <QVectorIterator>
#include <QPushButton>
#include <QComboBox>
#include <QProgressBar>
#include <QDir>
#include <qtconcurrentrun.h>
#include <QDirIterator>
#include <QFuture>
#include <QLabel>
#include <QTimer>

#include "copyworker.h"
#include "dbmanager.h"
#include "projectwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void StopWorker();
    void UpdateGUI(QTreeWidgetItem *, QStringList);

public slots:
    void getPath(QString, QString);
    void getItem(QTreeWidgetItem *);
    void getProgress(QProgressBar *, int);
    void getFinished(QTreeWidgetItem *);

private slots:
    void on_actionNeues_Projekt_triggered();
    void on_actionDeleteRoot_triggered();
    void on_actionDeleteChild_triggered();
    void on_syncButton_clicked();
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);
    void UpdateGUI_Triggered(QTreeWidgetItem *, QStringList);

private:
    void fetchProjects();
    Ui::MainWindow *ui;
    ProjectWidget *ProjektWidget;
    QVector<QTreeWidgetItem*> *TreeItemList;
    QVector<QTreeWidgetItem*> *RootItemList;
    QVector<CopyWorker*> *WorkerThreadList;
    CopyWorker *WorkerThread;
    DBManager *MyDBManager;
    QPoint ContextHelper;

};
#endif // MAINWINDOW_H
