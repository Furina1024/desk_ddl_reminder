#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QStandardItemModel>
QT_BEGIN_NAMESPACE
namespace Ui {
class Mainwindow;
class Mainwindow2;
class Mainwindow3;
class Mainwindow4;
class credits;
class history;
class selection;
class todolist;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QStandardItemModel *model = new QStandardItemModel;
    QString username;
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    QStackedWidget* StackedWidget;
    Ui::Mainwindow* ui;
};
#endif // MAINWINDOW_H
