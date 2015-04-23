#include "textedit.h"


ChangeHighlighter::ChangeHighlighter(QWidget* parent):
	m_parent(parent),
//	m_palDefault(m_parent->palette()),
//	m_palHightlight(m_palDefault),
	m_fontDefault(m_parent->font()),
	m_fontHighlight(m_fontDefault),
	m_highlight(false),
	m_changed(false)
{
	//m_palHightlight.setColor(QPalette::Text, QColor(255, 255, 0, 128));
	m_fontHighlight.setBold(true);
}

void ChangeHighlighter::track(bool f_track)
{
	m_changed = false;
	m_highlight = f_track;
	//m_parent->setPalette(m_palDefault);
	m_parent->setFont(m_fontDefault);
}

void ChangeHighlighter::onChange()
{
	if(!m_highlight || m_changed)
		return;
	//m_parent->setPalette(m_palHightlight);
	m_parent->setFont(m_fontHighlight);
	m_changed = true;
}

// ============================================================================
TextEdit::TextEdit(QWidget* parent):
	QTextEdit(parent),
	m_tracker(this)
{
	connect((QTextEdit*)this, SIGNAL(textChanged()), &m_tracker, SLOT(onChange()));
}

// ============================================================================
GenreBox::GenreBox(QWidget* parent):
	QComboBox(parent),
	m_tracker((QComboBox*)this)
{
	// Label actions
	connect((QComboBox*)this, SIGNAL(editTextChanged(const QString&)),
						this, SLOT  (onTextChange   (const QString&)));
	connect((QComboBox*)this, SIGNAL(currentIndexChanged(int)),
						this, SLOT  (onSelectionChange  (int)));
	// Text change tracker actions
	connect((QComboBox*)this, SIGNAL(editTextChanged(const QString&)),
				  &m_tracker, SLOT  (onChange       ()));
	connect((QComboBox*)this, SIGNAL(currentIndexChanged(int)),
				  &m_tracker, SLOT  (onChange           ()));
}


void GenreBox::onTextChange(const QString& str)
{
	onSelectionChange( findText(str) );
}

void GenreBox::onSelectionChange(int f_index)
{
	if(f_index == -1)
		m_label->clear();
	else
		m_label->setText( QString("(%1)").arg(f_index) );
}
