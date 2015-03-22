#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

namespace Ui {
class Window;
}

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);
    ~Window();

private slots:
    void on_comboGenre_editTextChanged(const QString& arg1);

private:
    Ui::Window *ui;
};

#endif // WINDOW_H
