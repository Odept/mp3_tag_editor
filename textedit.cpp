#include "textedit.h"


void TextEdit::onTextChange()
{
    QPalette p = palette();
    p.setColor(QPalette::Base, QColor(255, 216, 0, 32));
    setPalette(p);
}

// ============================================================================
void GenreBox::onTextChange(const QString& str)
{
    if(m_label.isNull())
        return;

    int i = findText(str);
    if(i == -1)
        m_label->clear();
    else
        m_label->setText( QString("(%1)").arg(i) );
}
