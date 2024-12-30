#ifndef COPYWORKER_H
#define COPYWORKER_H

#include <QObject>
#include <QThread>
#include <QProgressBar>
#include <QComboBox>
#include <QTreeWidgetItem>
#include <QDate>
#include <QFile>
#include <QDir>

class CopyWorker : public QObject
{
    Q_OBJECT
public:
    explicit CopyWorker(QObject *parent = nullptr);
    void start(QTreeWidgetItem *, QProgressBar *, QString, QString, QString);

signals:
    void sendProgress(QProgressBar *, int);
    void finished(QTreeWidgetItem *);

public slots:
    void stop();

private:
    bool stopped;
    static const int CHUNK_SIZE = 512 * 512;
};

#endif // COPYWORKER_H
