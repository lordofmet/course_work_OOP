#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./DB.db");
    if (db.open()) {
        qDebug("succesfully opened db\n");
    }
    else {
        qDebug("couldn't open db");
    }

    query = new QSqlQuery(db);
    query -> exec("DROP TABLE IF EXISTS SPBTheatres"); //DROP TABLE EVERY EXECUTION
    query -> exec("CREATE TABLE SPBTheatres(Название TEXT, Адрес TEXT, Дата_основания TEXT, ХудРук TEXT);");

    model = new QSqlTableModel(this, db);
    model -> setTable("SPBTheatres");
    model -> select();

    ui -> tableView -> setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_newFile_triggered()
{
    bool needSave = false;
    int ret = QMessageBox::Discard;

    if (model -> isDirty()) {
        ret = QMessageBox::question(this, tr("Сохранить"), tr("Вы хотите сохранить БД?"),
        QMessageBox::Save | QMessageBox::Save | QMessageBox::Cancel,
        QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            model -> submitAll();
            needSave = true;
        }
    }

    if (needSave) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл базы данных"), QDir::homePath(), tr("База данных (*.db)"));
        model -> submit();
        QFile::copy(db.databaseName(), fileName);
        qDebug("saved db\n");
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Создать новый файл"), QDir::homePath(), tr("База данных (*.db)"));

    if (!fileName.isEmpty()) {
        db.close();
        db.setDatabaseName(fileName);
        if (db.open()) {
            qDebug("opened db: creating new db\n");
        }
        else {
            qDebug("couldn't open db: creating new db\n");
            return;
        }
        query = new QSqlQuery(db);
        query -> exec("DROP TABLE IF EXISTS SPBTheatres");
        query -> exec("CREATE TABLE SPBTheatres(Название TEXT, Адрес TEXT, Дата_основания TEXT, ХудРук TEXT);");
        model -> setTable("SPBTheatres");
        model -> select();
    }

}

void MainWindow::on_action_add_triggered()
{
    model -> insertRow(model -> rowCount());
}


void MainWindow::on_action_del_triggered()
{
    model -> removeRow(row);
}


void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    row = index.row();
}


void MainWindow::on_action_triggered()
{

}

