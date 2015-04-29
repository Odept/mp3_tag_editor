#include "textedit.h"

#include "debug.h"


void ChangeHighlighter::track(bool f_track, bool f_changed)
{
	m_changed = false;
	m_track = f_track;

	ASSERT(f_track || !f_changed);
	if(f_changed)
		onChange();
	else
	{
		ASSERT(m_frame);
		m_frame->setStyleSheet( QString() );
	}
}

void ChangeHighlighter::onChange()
{
	if(!m_track || m_changed)
		return;
	ASSERT(m_frame);
	m_frame->setStyleSheet( QString("#%1 { border: 1px solid yellow; }").arg(m_frame->objectName()) );
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
	m_tracker(NULL)
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
