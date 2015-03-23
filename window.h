#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <QMainWindow>

namespace Ui {
class Window;
class TextEdit;
}

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);
    ~Window();

private:
    Ui::Window *ui;
};

#endif // __WINDOW_H__
