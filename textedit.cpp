#include "textedit.h"
#include "ui_window.h"


TextEdit::TextEdit(QWidget* parent):
	QTextEdit(parent),
	m_palDefault(palette()),
	m_palHightlight(m_palDefault),
	m_highlight(false)
{
	m_palHightlight.setColor(QPalette::Base, QColor(255, 255, 0, 128));

	connect((QTextEdit*)this, SIGNAL(textChanged()), this, SLOT(onTextChange()));
}


void TextEdit::trackChanges(bool f_track)
{
	m_highlight = f_track;
	setPalette(m_palDefault);
}

void TextEdit::onTextChange()
{
	if(!m_highlight)
		return;
	setPalette(m_palHightlight);
}

// ============================================================================
GenreBox::GenreBox(QWidget* parent):
	QComboBox(parent)
{
	connect((QComboBox*)this, SIGNAL(editTextChanged(const QString&)),
						this, SLOT  (onTextChange   (const QString&)));
}


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

// ============================================================================
TagBox::TagBox(QWidget* parent):
	QComboBox(parent)
{
	QStringList tags( QList<QString>() << "Combined" << "ID3v1" << "ID3v2" );
	addItems(tags);
}
