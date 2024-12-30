#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include <QDialog>
#include <QDir>

namespace Ui {
class projectWidget;
}

class ProjectWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectWidget(QWidget *parent = nullptr);
    ~ProjectWidget();

signals:
    void sendPath(QString, QString);

private slots:
    void on_pushButton_Break_clicked();
    void on_pushButton_OK_clicked();

private:
    Ui::projectWidget *ui;
};

#endif // PROJECTWIDGET_H
