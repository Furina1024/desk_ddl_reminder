#ifndef LOGIN_H
#define LOGIN_H
#include <QMainWindow>
QT_BEGIN_NAMESPACE
namespace Ui {
class login;
}
QT_END_NAMESPACE

class login : public QMainWindow
{
    Q_OBJECT

public:
    login(QWidget *parent = nullptr);
    ~login();
    void updatePosition() {
        if (parentWidget()) {
            move(parentWidget()->pos() + QPoint(100, 100));  // 相对父窗口偏移 (100,100)
        }
    }

private:
    Ui::login *ui;

};
#endif // LOGIN_H
