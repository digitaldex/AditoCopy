#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTreeWidgetItem>
#include <QTreeWidget>

class DBManager : public QObject
{
    Q_OBJECT
public:
    explicit DBManager(QObject *parent = nullptr);
    void openDatabase();
    void closeDatabase();
    void addProject(QString *, QString *);
    void queryProjects(QVector<QTreeWidgetItem *> *, QTreeWidget *);
    void deleteProject(QString);

signals:

public slots:

private:
    QSqlDatabase sqlitedb;
};

#endif // DBMANAGER_H
