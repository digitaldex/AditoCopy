#include "dbmanager.h"

DBManager::DBManager(QObject *parent) : QObject(parent) {

}

void DBManager::closeDatabase() {
    sqlitedb.close();
}

void DBManager::addProject(QString *name, QString *path) {
    QSqlQuery query;
    query.prepare("INSERT INTO projects (name, path) VALUES (:name, :path)");
    query.bindValue(":name", *name);
    query.bindValue(":path", *path);
    query.exec();
}

void DBManager::queryProjects(QVector<QTreeWidgetItem *> *rootItemList, QTreeWidget *treeWidget) {
    QSqlQuery query;
    query.exec("SELECT * FROM projects");
    while(query.next()) {
        QTreeWidgetItem *rootItem = new QTreeWidgetItem(treeWidget);
        rootItemList->append(rootItem);
        rootItem->setIcon(0, QIcon(":/icons/document-open-folder.png"));
        rootItem->setText(0, query.value(1).toString());
        rootItem->setText(1, query.value(2).toString());
        rootItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
    }
}

void DBManager::deleteProject(QString projectName) {
    QSqlQuery query;
    query.prepare("DELETE FROM projects WHERE name LIKE (:name)");
    query.bindValue(":name", projectName);
    query.exec();
}

void DBManager::openDatabase() {
    sqlitedb = QSqlDatabase::addDatabase("QSQLITE");
    sqlitedb.setDatabaseName("aditoCopy.sqlite");

    if(!sqlitedb.open()) {
        // qDebug() << "Error Opening Database";
    } else {
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS projects (id INT PRIMARY KEY, name VARCHAR, path VARCHAR)");
    }
}
