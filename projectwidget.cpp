#include "projectwidget.h"
#include "ui_projectwidget.h"

ProjectWidget::ProjectWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::projectWidget) {
    ui->setupUi(this);
}

ProjectWidget::~ProjectWidget() {
    delete ui;
}
void ProjectWidget::on_pushButton_Break_clicked() {
    ui->projectName->setText("");
    ui->aditoPath->setText("");
    close();
}

void ProjectWidget::on_pushButton_OK_clicked() {
    emit sendPath(ui->projectName->text(), QDir::toNativeSeparators(ui->aditoPath->text()));
    ui->projectName->setText("");
    ui->aditoPath->setText("");
    close();
}
