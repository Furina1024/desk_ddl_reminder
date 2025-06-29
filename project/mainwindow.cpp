#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./ui_mainwindow2.h"
#include "./ui_mainwindow3.h"
#include "./ui_mainwindow4.h"
#include "./ui_credits.h"
#include "./ui_history.h"
#include "./ui_selection.h"
#include "./ui_todolist.h"
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFile>
#include <QMap>
#include <QDataStream>
#include <QDialog>
#include <QDateTime>
#include <QList>
#include <QListView>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent),
    player(new QMediaPlayer(this)),
    videoWidget(new QVideoWidget(this)),
    ui(new Ui::Mainwindow),
    StackedWidget(new QStackedWidget(this))

{
    ui->setupUi(this);
    player->setVideoOutput(videoWidget);
    videoWidget->hide();
    QWidget* page2=new QWidget(StackedWidget);
    Ui::Mainwindow2 mw2;
    mw2.setupUi(page2);
    mw2.verticalLayout->addWidget(videoWidget);
    StackedWidget->addWidget(page2);
    QWidget* page3=new QWidget(StackedWidget);
    Ui::Mainwindow3 mw3;
    mw3.setupUi(page3);
    StackedWidget->addWidget(page3);
    QWidget* page4=new QWidget(StackedWidget);
    Ui::Mainwindow4 mw4;
    mw4.setupUi(page4);
    StackedWidget->addWidget(page4);
    QWidget* selection_page=new QWidget(StackedWidget);
    Ui::Selection selection;
    selection.setupUi(selection_page);
    StackedWidget->addWidget(selection_page);
    QWidget* credits_page=new QWidget(StackedWidget);
    Ui::credits credits;
    credits.setupUi(credits_page);
    StackedWidget->addWidget(credits_page);
    QWidget* history_page=new QWidget(StackedWidget);
    Ui::history history;
    history.setupUi(history_page);
    StackedWidget->addWidget(history_page);
    QWidget* todolist_page=new QWidget(StackedWidget);
    Ui::todolist todolist;
    todolist.setupUi(todolist_page);
    StackedWidget->addWidget(todolist_page);

    // 切换信号连接
    connect(ui->pushButton, &QPushButton::clicked, this, [this] {
        QString content1 = ui->username->text();
        username=content1;
        QString content2 = ui->password->text();
        QMap<QString, QString> map;
        QFile file("user-password.bin");
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        in >> map;
        file.close();
        auto it = map.find(content1);
        if (it != map.end()) {
            if(map[content1]==content2){
                setCentralWidget(StackedWidget);
                StackedWidget->setCurrentIndex(0);
                player->setSource(QUrl("qrc:/videos/video.mp4")); // 设置视频文件路径
                videoWidget->show(); // 显示窗口
                videoWidget->setAspectRatioMode(Qt::IgnoreAspectRatio);  // 拉伸填满
                player->play();      // 开始播放
                if(!QFile::exists("lastuser.bin")){
                    QFile file("lastuser.bin");
                    file.open(QIODevice::WriteOnly);
                    QDataStream out(&file);
                    out << content1;
                    file.close();
                }
                else{
                    QFile file("lastuser.bin");
                    file.open(QIODevice::WriteOnly);
                    QDataStream out(&file);
                    out << content1;
                    file.close();
                }
            }
            else{
                QDialog *dialog = new QDialog(this);
                dialog->setWindowTitle("提示");
                dialog->setModal(true);
                dialog->resize(300, 200);
                QLabel *label = new QLabel("密码错误！", dialog);
                QVBoxLayout *layout = new QVBoxLayout(dialog);
                layout->addWidget(label);
                dialog->exec();
            }
        }
        else{
            map.insert(content1,content2);
            file.open(QIODevice::WriteOnly);
            QDataStream out(&file);
            out << map;
            file.close();
            if(!QFile::exists("lastuser.bin")){
                QFile file("lastuser.bin");
                file.open(QIODevice::WriteOnly);
                QDataStream out(&file);
                out << content1;
                file.close();
            }
            else{
                QFile file("lastuser.bin");
                file.open(QIODevice::WriteOnly);
                QDataStream out(&file);
                out << content1;
                file.close();
            }
            setCentralWidget(StackedWidget);
            StackedWidget->setCurrentIndex(0);
            player->setSource(QUrl("qrc:/videos/video.mp4")); // 设置视频文件路径
            videoWidget->show(); // 显示窗口
            videoWidget->setAspectRatioMode(Qt::IgnoreAspectRatio);  // 拉伸填满
            player->play();      // 开始播放
        }
    });
    connect(player, &QMediaPlayer::playbackStateChanged,this, [this](QMediaPlayer::PlaybackState state){
        if (state == QMediaPlayer::StoppedState) {
            StackedWidget->setCurrentIndex(1);
        }
    });
    connect(page3->findChild<QPushButton*>("start"), &QPushButton::clicked, this, [this](){
        StackedWidget->setCurrentIndex(2);
    });//选择界面
    connect(page4->findChild<QPushButton*>("selection_button"), &QPushButton::clicked, this, [this](){
        StackedWidget->setCurrentIndex(3);
    });//选择角色界面
    connect(page4->findChild<QPushButton*>("credits_button"), &QPushButton::clicked, this, [this,credits](){
        if(!QFile::exists("points.bin")){
            QFile file("points.bin");
            file.open(QIODevice::WriteOnly);
            QMap<QString,int> map;
            map.insert(username,0);
            QDataStream out(&file);
            out << map;
        }
        QFile file1("points.bin");
        QMap<QString,int> points;
        file1.open(QIODevice::ReadOnly);
        QDataStream in(&file1);
        in >> points;
        file1.close();
        credits.level->setText(QString("您现在的等级是%1级").arg(points[username]/5+1));
        credits.points->setText(QString("您距离下一级还差%1点积分").arg(((points[username]/5)+1)*5-points[username]));
        StackedWidget->setCurrentIndex(4);
    });//签到界面
    connect(credits_page->findChild<QPushButton*>("sign"),&QPushButton::clicked, this, [this](){
        QDate currentDate = QDate::currentDate();
        if(!QFile::exists("signed_date.bin")){
            QFile file("signed_date.bin");
            file.open(QIODevice::WriteOnly);
            QDate time1(2006,6,30);
            QMap<QString,QDate> map;
            map.insert(username,time1);
            QDataStream out(&file);
            out << map;
        }
        QFile file("signed_date.bin");
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        QMap<QString,QDate> map;
        in >> map;
        file.close();
        if(map[username]==currentDate){
            QDialog *dialog = new QDialog(this);
            dialog->setWindowTitle("提示");
            dialog->setModal(true);
            dialog->resize(300, 200);
            QLabel *label = new QLabel("您今天已经签过到了哦~", dialog);
            QVBoxLayout *layout = new QVBoxLayout(dialog);
            layout->addWidget(label);
            dialog->exec();
        }
        else{
            file.open(QIODevice::WriteOnly);
            QDataStream out(&file);
            QMap<QString,QDate> m;
            m.insert(username,currentDate);
            out << m;
            file.close();
            QFile file1("points.bin");
            QMap<QString,int> points;
            file1.open(QIODevice::ReadOnly);
            QDataStream in(&file1);
            in >> points;
            file1.close();
            points[username]++;
            file1.open(QIODevice::WriteOnly);
            QDataStream out1(&file1);
            out1 << points;
            file1.close();
            QDialog *dialog = new QDialog(this);
            dialog->setWindowTitle("提示");
            dialog->setModal(true);
            dialog->resize(300, 200);
            QLabel *label = new QLabel("签到成功~", dialog);
            QVBoxLayout *layout = new QVBoxLayout(dialog);
            layout->addWidget(label);
            dialog->exec();
        }

    });//签到界面签到
    connect(page4->findChild<QPushButton*>("history_button"), &QPushButton::clicked, this, [this,history](){
        StackedWidget->setCurrentIndex(5);
        model->clear();
        if(!QFile::exists("history.bin")){
            QFile file("history.bin");
            file.open(QIODevice::WriteOnly);
            file.close();
        }
        QFile file1("history.bin");
        file1.open(QIODevice::ReadOnly);
        QMap<QString,QList<QMap<QString,QList<QDate>>>> map;
        QDataStream in(&file1);
        in>>map;
        file1.close();
        QList<QMap<QString,QList<QDate>>> list=map[username];
        for (int i = 0; i < list.length(); ++i) {
            QString plan=list[i].firstKey();
            QString ddl=list[i].first()[0].toString("yyyy-MM-dd");
            QString tixingdate=list[i].first()[1].toString("yyyy-MM-dd");
            QString str="todo:"+plan+"      ddl:"+ddl+"     提醒时间:"+tixingdate;
            QStandardItem *item = new QStandardItem(str);
            model->appendRow(item);
        }
        history.listView->setModel(model);
    });//历史界面
    connect(page4->findChild<QPushButton*>("todolist_button"), &QPushButton::clicked, this, [this,todolist](){
        StackedWidget->setCurrentIndex(6);
        model->clear();
        if(!QFile::exists("data.bin")){
            QFile file("data.bin");
            file.open(QIODevice::WriteOnly);
            file.close();
        }
        QFile file1("data.bin");
        file1.open(QIODevice::ReadOnly);
        QMap<QString,QList<QMap<QString,QList<QDate>>>> map;
        QDataStream in(&file1);
        in>>map;
        file1.close();
        QList<QMap<QString,QList<QDate>>> list=map[username];
        for (int i = 0; i < list.length(); ++i) {
            QString plan=list[i].firstKey();
            QString ddl=list[i].first()[0].toString("yyyy-MM-dd");
            QString tixingdate=list[i].first()[1].toString("yyyy-MM-dd");
            QString str="todo:"+plan+"      ddl:"+ddl+"     提醒时间:"+tixingdate;
            QStandardItem *item = new QStandardItem(str);
            item->setCheckable(true); // 启用复选框
            item->setCheckState(Qt::Unchecked); // 默认未选中
            item->setData(i, Qt::UserRole + 1);
            model->appendRow(item);
        }
        todolist.listView->setModel(model);
    });//todolist界面
    connect(todolist_page->findChild<QPushButton*>("submit"),&QPushButton::clicked, this, [this,todolist](){
        QString plan=todolist.lineEdit->text();
        QDate ddl=todolist.ddl->date();
        QDate tixingdate=todolist.tixingdate->date();
        if(!QFile::exists("data.bin")){
            QFile file("data.bin");
            file.open(QIODevice::WriteOnly);
            file.close();
        }
        QFile file1("data.bin");
        file1.open(QIODevice::ReadOnly);
        QMap<QString,QList<QMap<QString,QList<QDate>>>> map;
        QDataStream in(&file1);
        in>>map;
        file1.close();//读出

        QFile file("data.bin");
        file.open(QIODevice::WriteOnly);
        QList<QDate> d;
        d.append(ddl);
        d.append(tixingdate);
        QMap<QString,QList<QDate>> m;
        m.insert(plan,d);
        map[username].append(m);
        QDataStream out(&file);
        out << map;
        file.close();//写入

        QFile file2("data.bin");
        file2.open(QIODevice::ReadOnly);
        QMap<QString,QList<QMap<QString,QList<QDate>>>> map2;
        QDataStream in2(&file2);
        in2>>map2;
        file2.close();
        QList<QMap<QString,QList<QDate>>> list=map[username];
        model->clear();
        for (int i = 0; i < list.length(); ++i) {
            QString plan=list[i].firstKey();
            QString ddl=list[i].first()[0].toString("yyyy-MM-dd");
            QString tixingdate=list[i].first()[1].toString("yyyy-MM-dd");
            QString str="todo:"+plan+"      ddl:"+ddl+"     提醒时间:"+tixingdate;
            QStandardItem *item = new QStandardItem(str);
            item->setCheckable(true); // 启用复选框
            item->setCheckState(Qt::Unchecked); // 默认未选中
            item->setData(i, Qt::UserRole + 1);
            model->appendRow(item);
        }
        todolist.listView->setModel(model);
        //更新列表
    });
    connect(model, &QStandardItemModel::itemChanged,this, [this](QStandardItem *item) {
        int hidden = item->data(Qt::UserRole + 1).toInt();
        QFile file1("data.bin");
        file1.open(QIODevice::ReadOnly);
        QMap<QString,QList<QMap<QString,QList<QDate>>>> map;
        QDataStream in(&file1);
        in>>map;
        file1.close();//读出
        QMap<QString,QList<QDate>> del=map[username][hidden];
        map[username].removeAt(hidden);

        QFile file("data.bin");
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);
        out << map;
        file.close();//写入

        if(!QFile::exists("history.bin")){
            QFile file("history.bin");
            file.open(QIODevice::WriteOnly);
            file.close();
        }
        QFile file3("history.bin");
        file3.open(QIODevice::ReadOnly);
        QMap<QString,QList<QMap<QString,QList<QDate>>>> map1;
        QDataStream in1(&file3);
        in1>>map1;
        file3.close();
        if(map1.find(username)==map1.end()){
            QList<QMap<QString,QList<QDate>>> tmp;
            tmp.append(del);
            map1.insert(username,tmp);
        }
        else{
            map1[username].append(del);
        }
        file3.open(QIODevice::WriteOnly);
        QDataStream out1(&file3);
        out1<<map1;
    });//完成
    connect(selection_page->findChild<QPushButton*>("back"), &QPushButton::clicked, this, [this](){
        StackedWidget->setCurrentIndex(2);
    });
    connect(credits_page->findChild<QPushButton*>("back"), &QPushButton::clicked, this, [this](){
        StackedWidget->setCurrentIndex(2);
    });
    connect(history_page->findChild<QPushButton*>("back"), &QPushButton::clicked, this, [this](){
        StackedWidget->setCurrentIndex(2);
    });
    connect(todolist_page->findChild<QPushButton*>("back"), &QPushButton::clicked, this, [this](){
        StackedWidget->setCurrentIndex(2);
    });
    connect(selection_page->findChild<QPushButton*>("character1"),&QPushButton::clicked, this, [this](){
        if(!QFile::exists("character.bin")){
            QFile file("character.bin");
            file.open(QIODevice::WriteOnly);
            file.close();
        }
        QFile file("character.bin");
        file.open(QIODevice::WriteOnly);
        QMap<QString,int> map;
        map.insert(username,1);
        QDataStream out(&file);
        out<<map;
        file.close();
    });
    connect(selection_page->findChild<QPushButton*>("character2"),&QPushButton::clicked, this, [this](){
        if(!QFile::exists("character.bin")){
            QFile file("character.bin");
            file.open(QIODevice::WriteOnly);
            file.close();
        }
        QFile file("character.bin");
        file.open(QIODevice::WriteOnly);
        QMap<QString,int> map;
        map.insert(username,2);
        QDataStream out(&file);
        out<<map;
        file.close();
    });
    connect(selection_page->findChild<QPushButton*>("character3"),&QPushButton::clicked, this, [this](){
        if(!QFile::exists("character.bin")){
            QFile file("character.bin");
            file.open(QIODevice::WriteOnly);
            file.close();
        }
        QFile file("character.bin");
        file.open(QIODevice::WriteOnly);
        QMap<QString,int> map;
        map.insert(username,3);
        QDataStream out(&file);
        out<<map;
        file.close();
    });
    connect(selection_page->findChild<QPushButton*>("character4"),&QPushButton::clicked, this, [this](){
        if(!QFile::exists("character.bin")){
            QFile file("character.bin");
            file.open(QIODevice::WriteOnly);
            file.close();
        }
        QFile file("character.bin");
        file.open(QIODevice::WriteOnly);
        QMap<QString,int> map;
        map.insert(username,4);
        QDataStream out(&file);
        out<<map;
        file.close();
    });
    connect(selection_page->findChild<QPushButton*>("character5"),&QPushButton::clicked, this, [this](){
        if(!QFile::exists("character.bin")){
            QFile file("character.bin");
            file.open(QIODevice::WriteOnly);
            file.close();
        }
        QFile file("character.bin");
        file.open(QIODevice::WriteOnly);
        QMap<QString,int> map;
        map.insert(username,5);
        QDataStream out(&file);
        out<<map;
        file.close();
    });
    connect(selection_page->findChild<QPushButton*>("character6"),&QPushButton::clicked, this, [this](){
        if(!QFile::exists("character.bin")){
            QFile file("character.bin");
            file.open(QIODevice::WriteOnly);
            file.close();
        }
        QFile file("character.bin");
        file.open(QIODevice::WriteOnly);
        QMap<QString,int> map;
        map.insert(username,6);
        QDataStream out(&file);
        out<<map;
        file.close();
    });
    connect(history_page->findChild<QPushButton*>("delete1"),&QPushButton::clicked, this, [this,history](){
        QFile file("history.bin");
        file.open(QIODevice::WriteOnly);
        file.resize(0);
        file.close();
        model->clear();
        history.listView->setModel(model);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
