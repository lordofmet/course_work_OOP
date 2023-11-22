#include "mainwindow.h"
#include "my_vector.h"
#include "my_vector.hpp"
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
    //query -> exec("DROP TABLE IF EXISTS SPBTheatres"); //DROP TABLE EVERY EXECUTION
    //arr<QString> v(0, 10);
    v.add("Название");
    v.add("Адрес");
    v.add("Дата_основания");
    v.add("ХудРук");
    v.add("SPBTheatres");
    auto it = v.begin();
    QString new_db = "CREATE TABLE " + *(it + 4) + "(" + *it + " TEXT, " + *(it + 1) + " TEXT, " + *(it + 2) + " TEXT, "
                     + *(it + 3) + " TEXT);";
    qDebug() << new_db << "\n";
    query -> exec(new_db);
    //query->exec("CREATE TABLE SPBTheatres(" + *it + " TEXT, " + *(it + 1) + " TEXT, " + *(it + 2) + " TEXT, " + *(it + 3) + " TEXT);");
    //query -> exec("CREATE TABLE SPBTheatres(Название TEXT, Адрес TEXT, Дата_основания TEXT, ХудРук TEXT);");

    model = new QSqlTableModel(this, db);
    model -> setTable("SPBTheatres");
    model -> select();

    ui -> tableView -> setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::checkSave() {
    bool needSave = false;
    int ret = QMessageBox::Discard;

    if (model -> isDirty()) {
        ret = QMessageBox::question(this, tr("Сохранить"), tr("Вы хотите сохранить БД?"),
        QMessageBox::Save | QMessageBox::Cancel,
        QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            model -> submitAll();
            needSave = true;
        }
    }

    if (needSave) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл базы данных"),
                           QDir::homePath(), tr("База данных (*.db)"));
        model -> submit();
        QFile::copy(db.databaseName(), fileName);
        qDebug("saved db\n");
    }
}

void MainWindow::on_action_newFile_triggered()
{
    statusBar()->showMessage("Выполняется создание нового файла...");
    checkSave();

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
        delete query;
        query = new QSqlQuery(db);
        query -> exec("DROP TABLE IF EXISTS SPBTheatres");
        query -> exec("CREATE TABLE SPBTheatres(Название TEXT, Адрес TEXT, Дата_основания TEXT, ХудРук TEXT);");
        model -> setTable("SPBTheatres");
        model -> select();
    }
    statusBar()->showMessage("");
}

void MainWindow::on_action_add_triggered()
{
    model -> insertRow(model -> rowCount());
}


void MainWindow::on_action_del_triggered()
{
    model -> removeRow(row);
    model -> select();
}


void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    row = index.row();
}

void MainWindow::on_action_open_triggered()
{
    statusBar()->showMessage("Выполняется открытие БД...");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть файл"), QDir::homePath(), tr("База данных (*.db)"));
    if (!fileName.isEmpty()) {
        checkSave();
        db.close();
        db.setDatabaseName(fileName);
        if (db.open()) {
            qDebug("reopened DB\n");
        }
        else {
            qDebug("failed to reopen db\n");
            return;
        }
        delete query;
        query = new QSqlQuery(db);
        model -> setTable("SPBTheatres");
        model -> select();
    }
    statusBar()->showMessage("");
}

void MainWindow::on_action_save_triggered()
{
    statusBar()->showMessage("Выполняется сохранение...");
    model -> submit();
    QString currentDir = QDir::currentPath();
    QString fileName = currentDir;
    QFile::copy(db.databaseName(), fileName);
    qDebug("Saved in prev dir\n");
    statusBar()->showMessage("БД была сохранена");
}

void MainWindow::on_action_saveAs_triggered()
{
    statusBar()->showMessage("Выполняется сохранение...");
    model -> submit();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл базы данных"),
                       QDir::homePath(), tr("База данных (*.db)"));
    if (!fileName.isEmpty()) {
        QFile::copy(db.databaseName(), fileName);
        qDebug("saved in new dir\n");
    }
    qDebug("empty file\n");
    statusBar()->showMessage("БД была сохранена");
}

void MainWindow::on_action_about_triggered()
{
    statusBar()->showMessage("Выполняется показ информации о проекте...");
    QMessageBox::question(this, tr("Информация"), tr("Курсовая работа ИКПИ-22 по дисциплине ООП"),
    QMessageBox::Ok, QMessageBox::Cancel);
    statusBar()->showMessage("");
}

void MainWindow::on_action_merge_triggered()
{
    statusBar()->showMessage("Выполняется сляние...");
    QString secondDbFile = QFileDialog::getOpenFileName(this, tr("Выберите базу данных для слияния"),
                           QDir::homePath(), tr("База данных(*.db)"));
    if (!secondDbFile.isEmpty()) {
        QSqlDatabase secondDb = QSqlDatabase::addDatabase("QSQLITE", "second_connection");
        secondDb.setDatabaseName(secondDbFile);
        if (secondDb.open()) {
            QSqlQuery *secondQuery = new QSqlQuery(secondDb);
            secondQuery -> exec("SELECT * FROM SPBTheatres");
            while(secondQuery -> next()) {
                QString theatreName = secondQuery -> value("Название").toString();
                if (!theatreExists(theatreName)) {
                    QSqlRecord record = model -> record();
                    record.setValue("Название", theatreName);
                    record.setValue("Адрес", secondQuery -> value("Адрес"));
                    record.setValue("Дата_основания", secondQuery -> value("Дата_основания"));
                    record.setValue("ХудРук", secondQuery -> value("ХудРук"));
                    model -> insertRecord(-1, record);
                }
            }
            model -> submitAll();
            delete secondQuery;
            secondDb.close();
            model -> select();
        }
        else {
            qDebug("unable to open db for merge\n");
        }
    }
    else {
        qDebug("unable to choose db for merge\n");
    }
    statusBar()->showMessage("");
}

bool MainWindow::theatreExists(const QString &theatreName) {
    for (int row = 0; row < model -> rowCount(); row++) {
        QModelIndex index = model -> index(row, 0);
        if (model -> data(index).toString() == theatreName) {
            return true;
        }
    }
    return false;
}

void MainWindow::search(const QString &search_q) {
    qDebug() << "searching" << tmp + 1;
    tmp++;
    QSqlTableModel *searchModel = new QSqlTableModel(this, db);
    auto it = v.begin();
    searchModel -> setTable(*(it + 4));
    QString filter = *it + " LIKE '%" + search_q + "%' OR " + *(it + 1) + " LIKE '%" + search_q
    + "%' OR " + *(it + 2) + " LIKE '%" + search_q + "%' OR " + *(it + 3) + " LIKE '%" + search_q + "%'";
    qDebug() << "filter = " << filter << "\n";
    searchModel -> setFilter(filter);
    searchModel -> select();
    ui -> tableView -> setModel(searchModel);
    statusBar()->showMessage("Выполняется поиск в базе данных...");
}

void MainWindow::on_lineEdit_textEdited(const QString &arg1)
{
    statusBar()->showMessage("Вводится текстовый запрос");
    search_query = arg1;
    if (search_query.size() == 0) {
        QString filter = "";
        model -> setFilter(filter);
        model -> select();
        ui -> tableView -> setModel(model);
    }
    statusBar()->showMessage("");
}

void MainWindow::on_searchButton_clicked()
{
    MainWindow::search(search_query);
}

