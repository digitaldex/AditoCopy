#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ProjektWidget = new ProjectWidget;
    TreeItemList = new QVector<QTreeWidgetItem*>;
    RootItemList = new QVector<QTreeWidgetItem*>;
    WorkerThreadList = new QVector<CopyWorker*>;
    MyDBManager = new DBManager;
    MyDBManager->openDatabase();
    ui->treeWidget->setColumnWidth(0, 200);
    ui->treeWidget->setColumnWidth(1, 150);
    ui->treeWidget->setColumnWidth(2, 300);
    connect(ProjektWidget, SIGNAL(sendPath(QString, QString)), this, SLOT(getPath(QString, QString)));
    connect(ui->treeWidget, SIGNAL(addItem(QTreeWidgetItem *)), this, SLOT(getItem(QTreeWidgetItem *)));
    connect(this, SIGNAL(UpdateGUI(QTreeWidgetItem *, QStringList)), this, SLOT(UpdateGUI_Triggered(QTreeWidgetItem *, QStringList)));
    fetchProjects();
}

MainWindow::~MainWindow() {
    MyDBManager->closeDatabase();
    emit StopWorker();
    delete ProjektWidget;
    QVectorIterator<QTreeWidgetItem*> it(*TreeItemList);
    while(it.hasNext()) {
        QTreeWidgetItem *childItem(it.next());
        delete childItem;
    }
    delete TreeItemList;
    QVectorIterator<QTreeWidgetItem*> it2(*RootItemList);
    while(it2.hasNext()) {
        QTreeWidgetItem *childItem(it2.next());
        delete childItem;
    }
    delete RootItemList;
    QVectorIterator<CopyWorker*> it3(*WorkerThreadList);
    while(it3.hasNext()) {
        CopyWorker *childWorker(it3.next());
        delete childWorker;
    }
    delete WorkerThreadList;
    delete MyDBManager;
    delete ui;
}

/* Slot für das sendPath Signal aus der projectWidget Klasse.
 * Beim erstellen eines neuen Projekts über den projectWidget Dialog
 * werden die eingetragenen Daten (Name und Pfad) hierher übergeben und
 * ein neues Root Item erzeugt.
 * Der Zeiger auf das neue Item wird in einem Vector gespeichert und
 * zusätzlich über die dbManager Klasse in eine sqlite DB geschrieben damit
 * bei einem erneuten Programmstart die bereits hinzugefügten Projekte wieder
 * da sind
*/
void MainWindow::getPath(QString project, QString adito) {
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->treeWidget);
    RootItemList->append(rootItem);
    rootItem->setIcon(0, QIcon(":/icons/document-open-folder.png"));
    rootItem->setText(0, project);
    rootItem->setText(1, adito);
    rootItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
    MyDBManager->addProject(&project, &adito);
}

// Aufruf für den neues Projekt Dialog
void MainWindow::on_actionNeues_Projekt_triggered() {
    ProjektWidget->show();
}

/* Slot für das sendItem Signal aus der filestreewidget Klasse.
 * Werden über die Klasse externe Dateien in den Tree gedroppt, wird für
 * jede Datei ein Item erzeugt. Jeder ItemZeiger wird in einem Vector gespeichert
 * und die beiden ItemWidget ComboBox und Progressbar eingesetzt.
*/
void MainWindow::getItem(QTreeWidgetItem *item) {
    QLabel *label = new QLabel;
    label->setText("Ordnerstruktur wird gelesen.");
    ui->treeWidget->setItemWidget(item, 2, label);
    
    // Timer für die Animation erstellen
    QTimer *timer = new QTimer(this);
    int dots = 1;
    connect(timer, &QTimer::timeout, [label, dots]() mutable {
        QString text = "Ordnerstruktur wird gelesen";
        text += QString(".").repeated(dots);
        label->setText(text);
        dots = dots >= 3 ? 1 : dots + 1;
    });
    timer->start(500); // Alle 500ms aktualisieren
    
    QFuture<void> future = QtConcurrent::run([=]() {
        QStringList directoryListing;
        directoryListing << "Zielordner wählen.." << item->parent()->text(1);
        using ItFlag = QDirListing::IteratorFlag;
        for (const auto &dirEntry : QDirListing(item->parent()->text(1), ItFlag::Recursive | ItFlag::DirsOnly)) {
            directoryListing <<  QDir::toNativeSeparators(dirEntry.filePath());
        }
        timer->stop();
        timer->deleteLater();
        emit UpdateGUI(item, directoryListing);
    });
}

void MainWindow::UpdateGUI_Triggered(QTreeWidgetItem *item, QStringList directoryListing) {
    TreeItemList->append(item);
    QComboBox *combo = new QComboBox;
    QProgressBar *progress = new QProgressBar;
    combo->addItems(directoryListing);
    progress->setRange(0,100);
    ui->treeWidget->setItemWidget(item, 1, progress);
    ui->treeWidget->setItemWidget(item, 2, combo);
}

// Copyworker Slot zum Fortschritt des Kopiervorgangs
void MainWindow::getProgress(QProgressBar *progress, int value) {
    progress->setValue(value);
}

/* Copyworker Slot zum Ende des Kopiervorgangs
 * Nach Abschluss des kopierens wird das entsprechende
 * Childitem im Tree gelöscht
*/
void MainWindow::getFinished(QTreeWidgetItem *item) {
    TreeItemList->remove(TreeItemList->indexOf(item));
    delete item;
}

void MainWindow::on_actionDeleteChild_triggered() {
    emit StopWorker();
    QTreeWidgetItem *clickedItem = ui->treeWidget->itemAt(ContextHelper);
    TreeItemList->remove(TreeItemList->indexOf(clickedItem));
    delete ui->treeWidget->itemWidget(clickedItem, 1);
    delete ui->treeWidget->itemWidget(clickedItem, 2);
    delete clickedItem;
}

void MainWindow::on_actionDeleteRoot_triggered() {
    emit StopWorker();
    QTreeWidgetItem *clickedItem = ui->treeWidget->itemAt(ContextHelper);
    QVectorIterator<QTreeWidgetItem*> it(*TreeItemList);
    while(it.hasNext()) {
        QTreeWidgetItem *childItem(it.next());
        if(childItem->parent() == clickedItem) {
            TreeItemList->remove(TreeItemList->indexOf(childItem));
            delete ui->treeWidget->itemWidget(childItem, 1);
            delete ui->treeWidget->itemWidget(childItem, 2);
            delete childItem;
        }
    }
    MyDBManager->deleteProject(clickedItem->text(0));
    RootItemList->remove(RootItemList->indexOf(clickedItem));
    delete clickedItem;
}

void MainWindow::on_syncButton_clicked() {
    QVectorIterator<QTreeWidgetItem*> it(*TreeItemList);
    while(it.hasNext()) {
        WorkerThread = new CopyWorker;
        WorkerThreadList->append(WorkerThread);
        QTreeWidgetItem *item(it.next());
        QProgressBar *bar = qobject_cast<QProgressBar *>(ui->treeWidget->itemWidget(item, 1));
        QComboBox *box = qobject_cast<QComboBox *>(ui->treeWidget->itemWidget(item, 2));
        QString sourcePath(QDir::toNativeSeparators(item->data(0, Qt::UserRole).toString()));
        QString destPath(QDir::toNativeSeparators(box->currentText()+"\\"));
        QString fileName(item->text(0));
        QDir dir(destPath);

        if(box->currentText() == "Zielordner wählen..") {
            ui->statusbar->showMessage("Bitte wählen Sie einen Zielordner für "+fileName, 5000);
        } else if(!dir.exists()) {
            ui->statusbar->showMessage("Adito Ordner "+destPath+" nicht vorhanden oder Netzlaufwerk nicht erreichbar", 5000);
        } else {
            connect(WorkerThread, &CopyWorker::sendProgress, this, &MainWindow::getProgress);
            connect(WorkerThread, &CopyWorker::finished, this, &MainWindow::getFinished);
            connect(this, &MainWindow::StopWorker, WorkerThread, &CopyWorker::stop);
            QFuture<void> thread = QtConcurrent::run(&CopyWorker::start, WorkerThread, item, bar, sourcePath, destPath, fileName);
        }
    }
}

void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint &pos) {
    ContextHelper = pos;
    QTreeWidgetItem *clickedItem = ui->treeWidget->itemAt(pos);
    if(!clickedItem) {
        return;
    }
    if(!clickedItem->parent()) {
        QMenu menu(this);
        menu.addAction(ui->actionDeleteRoot);
        menu.exec(ui->treeWidget->mapToGlobal(pos));
    }
    if(clickedItem->parent()) {
        QMenu menu(this);
        menu.addAction(ui->actionDeleteChild);
        menu.exec(ui->treeWidget->mapToGlobal(pos));
    }
}

void MainWindow::fetchProjects() {
    MyDBManager->queryProjects(RootItemList, ui->treeWidget);
}
