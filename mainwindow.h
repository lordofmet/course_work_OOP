#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "my_vector.h"
#include "theatre.h"
#include <QGridLayout>
#include <QMainWindow>
#include <QSqlDatabase>
#include <QDebug>
#include <QVector>
#include <QSqlQuery>
#include <QFormLayout>
#include <QSqlTableModel>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlRecord>
#include <QChar>
#include <QModelIndex>
#include <QIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_newFile_triggered();

    void on_action_add_triggered();

    void on_action_del_triggered();

    void on_tableView_clicked(const QModelIndex &index);

    void checkSave();

    void on_action_open_triggered();

    void on_action_save_triggered();

    void on_action_about_triggered();

    void on_action_saveAs_triggered();

    void on_action_merge_triggered();

    bool theatreExists(const QString &theatreName);

    void on_lineEdit_textEdited(const QString &arg1);

    void search(const QString &search_q);

    void on_searchButton_clicked();

    void on_action_sortAlph_triggered();

    void on_action_sortRevAlph_triggered();

    void on_action_changeRec_triggered();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQuery *query;
    QSqlTableModel *model;
    QString search_query;
    arr<QString> v;
    arr<theatre> v1;

    int row, col;
    int tmp = 0;
};

#endif // MAINWINDOW_H
