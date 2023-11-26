#include "mainwindow.h"
#include "my_vector.h"
#include "my_vector.cpp"
#include "theatre.h"
#include "theatre.cpp"
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
    QString n = "Название", a = "Адрес", d = "Дата_основания", h = "ХудРук", dbName = "SPBTheatres";
    theatre new_t(n, a, d, h);
    v.add(new_t.name);
    v.add(new_t.adress);
    v.add(new_t.date);
    v.add(new_t.head);
    v.add(dbName);
    auto it = v.begin();
    QString new_db = "CREATE TABLE " + *(it + 4) + "(" + *it + " TEXT, " + *(it + 1) + " TEXT, " + *(it + 2) + " TEXT, "
                     + *(it + 3) + " TEXT);";
    qDebug() << new_db << "\n";
    query -> exec(new_db);

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
    statusBar()->showMessage("Создан новый файл");
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
    statusBar()->showMessage("Открыта БД");
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
    statusBar()->showMessage("Был выполнен показ данных о файле");
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
    statusBar()->showMessage("Было выполнено слияние БД");
}

void MainWindow::on_action_del_triggered()
{
    model -> removeRow(row);
    model -> select();
    v1[row].name = "";
    statusBar()->showMessage("Удалена запись");
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
        statusBar()->showMessage("Завершен поиск данных по поиску");
    }
    else
        statusBar()->showMessage("Показ данных по поиску");
}

void MainWindow::on_searchButton_clicked()
{
    MainWindow::search(search_query);
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    row = index.row();
    col = index.column();
    qDebug() << "new rol = " << col << "\n";
}

void MainWindow::on_action_sortAlph_triggered()
{
    model->setSort(col, Qt::AscendingOrder);
    model -> select();
    model->sort(col, Qt::AscendingOrder);
    model ->submitAll();
    model -> select();
    ui -> tableView -> setModel(model);
    statusBar()->showMessage("Отсортировано в алфавитном порядке");
}


void MainWindow::on_action_sortRevAlph_triggered()
{
    model->setSort(col, Qt::DescendingOrder);
    model -> select();
    model->sort(col, Qt::DescendingOrder);
    model ->submitAll();
    model -> select();
    ui -> tableView -> setModel(model);
    statusBar()->showMessage("Отсортировано в обратном алфавитном порядке");
}

void MainWindow::on_action_add_triggered()
{
    QDialog dialog;
    QGridLayout layout(&dialog);
    QLineEdit nameIn, adressIn, dateIn, directorIn;
    QLabel name("Название:"), adress("Адрес:"), date("Дата_основания:"), director("ХудРук:");

    layout.addWidget(&name, 0, 0);
    layout.addWidget(&nameIn, 0, 1);
    layout.addWidget(&adress, 1, 0);
    layout.addWidget(&adressIn, 1, 1);
    layout.addWidget(&date, 2, 0);
    layout.addWidget(&dateIn, 2, 1);
    layout.addWidget(&director, 3, 0);
    layout.addWidget(&directorIn, 3, 1);

    QPushButton add("Изменить");
    layout.addWidget(&add, 4, 0, 1, 2);

    connect(&add, &QPushButton::clicked, [&]() {
        QString tName, tAdress, tDate, tDirector;
        tName = nameIn.text();
        tAdress = adressIn.text();
        tDate = dateIn.text();
        tDirector = directorIn.text();

        int newRow = model -> rowCount();
        model ->insertRow(newRow);
        model->setData(model->index(newRow, 0), tName);
        model->setData(model->index(newRow, 1), tAdress);
        model->setData(model->index(newRow, 2), tDate);
        model->setData(model->index(newRow, 3), tDirector);

        theatre tmp(tName, tAdress, tDate, tDirector);
        v1.add(tmp);
        qDebug() << v1[v1.get_size() - 1].name << "\n";

        dialog.close();
    });

    dialog.exec();
    statusBar()->showMessage("Добавлена запись");
}

void MainWindow::on_action_changeRec_triggered()
{
    QDialog dialog;
    QFormLayout form(&dialog);
    QLineEdit nameEdit, addressEdit, dateEdit, directorEdit;

    int selectedRow = ui->tableView->selectionModel()->currentIndex().row();

    QSqlRecord record = model->record(selectedRow);

    QString sn, sa, sd, sdir;
    nameEdit.setText(record.value("Название").toString());
    addressEdit.setText(record.value("Адрес").toString());
    dateEdit.setText(record.value("Дата_основания").toString());
    directorEdit.setText(record.value("ХудРук").toString());

    form.addRow("Название:", &nameEdit);
    form.addRow("Адрес:", &addressEdit);
    form.addRow("Дата основания:", &dateEdit);
    form.addRow("Худрук:", &directorEdit);

    QPushButton okButton("Изменить");
    form.addRow(&okButton);

    sn = nameEdit.text();
    sa = addressEdit.text();
    sd = dateEdit.text();
    sdir = directorEdit.text();

    for (auto it : v1) {
        if (it.name == sn || it.adress == sa || it.date == sd || it.head == sdir) {
            it.name = sn;
            it.adress = sa;
            it.date = sd;
            it.head = sdir;
        }
    }

    connect(&okButton, &QPushButton::clicked, [&]() {
        model->setData(model->index(selectedRow, 0), nameEdit.text());
        model->setData(model->index(selectedRow, 1), addressEdit.text());
        model->setData(model->index(selectedRow, 2), dateEdit.text());
        model->setData(model->index(selectedRow, 3), directorEdit.text());
        model->submitAll();
        dialog.close();
    });

    dialog.exec();

    statusBar()->showMessage("Изменена запись");
}

