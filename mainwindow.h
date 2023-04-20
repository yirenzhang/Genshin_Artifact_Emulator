
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QRandomGenerator>
#include <QUuid>
#include <QListWidget>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT
private:
    QSqlDatabase DB;
    quint32 rand;

    QList<QString> IdList;
public:
    void CreateDB();
    void InitMap();
    void ReadId();
    void ListInit(quint32 num);
    void AddLegacy(QString MainStr);
    void GenerateInit();
    void Del();
    void Upgrade(QString ID);
    void RenewList(quint32 num=0);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_LegacyGen_clicked();
    void on_renew_clicked();
    void on_DeleteOne_clicked();
    void on_LegacyList_itemClicked(QListWidgetItem *item);
    void on_UpGrd_clicked();

private:
    Ui::MainWindow *ui;
    QMap <QString, QString> SubStrMap;

};

#endif // MAINWINDOW_H
