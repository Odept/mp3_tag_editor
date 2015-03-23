#include "window.h"
#include "ui_window.h"

#include "External/inc/genre.h"


Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);

    QStringList tags( QList<QString>() << "All" << "ID3v1" << "ID3v2" );
    ui->comboTag->addItems(tags);

    ui->comboGenre->connectLabel(ui->labelBoxGenre);
    for(unsigned int i = 0;; i++)
    {
        const char* szGenre = CGenre::get(i);
        if(!*szGenre)
            break;
        QString genre(szGenre);
        ui->comboGenre->addItem(genre);
    }
}

Window::~Window()
{
    delete ui;
}
