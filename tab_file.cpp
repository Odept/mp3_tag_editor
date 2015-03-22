#include "window.h"
#include "ui_window.h"


void Window::on_comboGenre_editTextChanged(const QString& text)
{
    int i = ui->comboGenre->findText(text);
    if(i == -1)
        ui->labelBoxGenre->clear();
    else
        ui->labelBoxGenre->setText( QString("(%1)").arg(i) );
}
