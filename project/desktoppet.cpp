#include "desktoppet.h"
#include "mainwindow.h"
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QFile>
#include <QTimer>
#include <QPropertyAnimation>
DesktopPet::DesktopPet(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    resize(200, 200);  // 设置窗口大小
    move(1400, 700);    // 设置初始位置
    if(QFile::exists("lastuser.bin")){
        QFile file("lastuser.bin");
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        in >> user;
        file.close();

        //写入text
        QFile file1("data.bin");
        file1.open(QIODevice::ReadOnly);
        QMap<QString,QList<QMap<QString,QList<QDate>>>> map;
        QDataStream in1(&file1);
        in1>>map;
        file1.close();//读出
        QList<QMap<QString,QList<QDate>>> list=map[user];
        bool flag=true;
        for(int i=0;i<list.length();i++){
            QDate t=list[i].first()[1];
            if(current>=t){
                if(flag){
                    flag=false;
                    m_speechTexts+="您还有以下任务没有完成哦~\n";
                    m_speechTexts+=QString("%1\nddl:%2\n").arg(list[i].firstKey(),list[i].first()[0].toString("yyyy-MM-dd"));
                }

            }
        }

        // 创建图形视图气泡
        m_bubbleView = new QGraphicsView(this);
        m_bubbleView->setStyleSheet("background: transparent; border: none;");
        m_bubbleView->setRenderHint(QPainter::Antialiasing);
        m_bubbleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_bubbleView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_bubbleView->setFixedSize(400, 200); // 固定大小
        m_bubbleView->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        m_bubbleView->setAttribute(Qt::WA_TranslucentBackground);
        m_bubbleView->raise();  // 确保在最上层
        m_bubbleView->hide();

        // 创建场景和气泡元素
        m_bubbleScene = new QGraphicsScene(this);
        m_bubbleView->setScene(m_bubbleScene);

        // 创建气泡背景（带箭头）
        QGraphicsPixmapItem *bubbleBg = new QGraphicsPixmapItem(
            QPixmap(":/images/speech_bubble.png") // 准备气泡图片资源
            );
        m_bubbleScene->addItem(bubbleBg);

        m_bubbleText = new QGraphicsTextItem();
        m_bubbleText->setDefaultTextColor(Qt::black);
        m_bubbleText->setTextWidth(700); // 限制宽度自动换行
        m_bubbleText->setPos(20, 20);
        m_bubbleScene->addItem(m_bubbleText);

        // 初始化动画
        m_fadeAnimation = new QPropertyAnimation(m_bubbleView, "windowOpacity", this);
        m_fadeAnimation->setDuration(500); // 动画时长500ms

        // 设置定时器
        m_speechTimer = new QTimer(this);
        connect(m_speechTimer, &QTimer::timeout, this, &DesktopPet::showSpeech);
        m_speechTimer->start(10000); // 每10秒触发一次
    }
    createTrayIcon();
    show();
}

void DesktopPet::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    QPixmap pix(":/images/furina.png");
    painter.drawPixmap(rect(), pix);
}

void DesktopPet::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void DesktopPet::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void DesktopPet::createTrayIcon() {
    QMenu *trayMenu = new QMenu(this);
    trayMenu->addAction("退出", qApp, &QCoreApplication::quit);
    QAction *settingsAction = trayMenu->addAction("打开设置");
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->setIcon(QIcon(":/images/furina.png"));
    trayIcon->show();
    connect(settingsAction, &QAction::triggered, this, [this]() {
        MainWindow *w = new MainWindow(this);
        w->setAttribute(Qt::WA_DeleteOnClose);  // 关闭时自动删除
        w->show();
    });
    connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            setVisible(!isVisible());
        }
    });
}

void DesktopPet::showSpeech() {
    if (m_speechTexts.isEmpty()) return;

    QFont font;
    font.setPointSize(34);  // 设置字号大小
    font.setFamily("Microsoft YaHei");  // 设置字体
    m_bubbleText->setFont(font);
    // 导入台词
    QString text=m_speechTexts;
    m_bubbleText->setPlainText(text);
    m_bubbleView->setSceneRect(m_bubbleScene->itemsBoundingRect());
    m_bubbleView->fitInView(m_bubbleScene->sceneRect(), Qt::KeepAspectRatio);

    // 定位气泡（在桌宠上方居中）
    QPoint petPos = this->mapToGlobal(QPoint(0, 0));
    QPoint bubblePos(
        petPos.x()-100,
        petPos.y()-m_bubbleView->height()
        );
    m_bubbleView->move(bubblePos);

    // 渐入动画
    m_bubbleView->setWindowOpacity(0); // 初始透明
    m_bubbleView->show();

    QPropertyAnimation *fadeIn = new QPropertyAnimation(m_bubbleView, "windowOpacity", this);
    fadeIn->setDuration(500);
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(1);
    fadeIn->start();

    // 3秒后启动渐出动画
    QTimer::singleShot(3000, this, [this]() {
        m_fadeAnimation->setDuration(500);
        m_fadeAnimation->setStartValue(1);
        m_fadeAnimation->setEndValue(0);
        connect(m_fadeAnimation, &QPropertyAnimation::finished,this, [this]() {
            m_bubbleView->hide();
        });
        m_fadeAnimation->start();
    });
}
