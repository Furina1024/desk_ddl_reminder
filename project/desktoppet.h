#ifndef DESKTOPPET_H
#define DESKTOPPET_H

#pragma once
#include <QWidget>
#include <QSystemTrayIcon>
#include <QDate>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QPropertyAnimation>
class DesktopPet : public QWidget {
    Q_OBJECT
public:
    explicit DesktopPet(QWidget *parent = nullptr);

protected:
    QDate current=QDate::currentDate();
    QString user;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
private slots:
    void showSpeech();
private:
    QPoint m_dragPosition;
    QTimer *m_speechTimer;
    QString m_speechTexts;
    QGraphicsView *m_bubbleView;
    QGraphicsScene *m_bubbleScene;
    QGraphicsPixmapItem *m_bubbleBg;
    QGraphicsTextItem *m_bubbleText;
    QPropertyAnimation *m_fadeAnimation;
    void createTrayIcon();
};

#endif // DESKTOPPET_H
