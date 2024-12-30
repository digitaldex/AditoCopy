#include "copyworker.h"

CopyWorker::CopyWorker(QObject *parent) : QObject(parent)
{
    stopped = false;
}

void CopyWorker::start(QTreeWidgetItem *item, QProgressBar *progress, QString sourcePath, QString destPath, QString fileName) {
    stopped = false;
    QString currentDate(QDate::currentDate().toString("yyyy-MM-dd"));
    fileName.prepend(currentDate+"_");
    QFile sourceFile(sourcePath);
    QFile destFile(destPath+fileName);
    qint64 total = 0, written = 0;
    int done = 0;

    if (QFile::exists(destPath+fileName)) {
        QFile::remove(destPath+fileName);
    }
    if (!sourceFile.open(QFileDevice::ReadOnly)) {
        return;
    }
    if (!destFile.open(QFileDevice::WriteOnly)) {
        return;
    }

    qint64 fSize = sourceFile.size();
    total = fSize;
    while(fSize) {
        if(stopped) {
            sourceFile.close();
            destFile.close();
            return;
        }
        const auto data = sourceFile.read(CHUNK_SIZE);
        const auto _written = destFile.write(data);
        if (data.size() == _written) {
            written += _written;
            fSize -= data.size();
            done = (int)((written * 100) / total);
            emit sendProgress(progress, done);
        } else {
            break;
        }
    }

    sourceFile.close();
    destFile.close();

    if (total == written) {
        emit finished(item);
    } else {
        return;
    }
}

void CopyWorker::stop() {
    stopped = true;
}
