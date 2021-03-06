#ifndef __TEXTEDIT_H__
#define __TEXTEDIT_H__

#include "debug.h"

#include <QTextEdit>
#include <QComboBox>
#include <QLabel>


class ChangeHighlighter : public QObject
{
	Q_OBJECT

public:
	explicit ChangeHighlighter(QFrame* f_frame):
		m_frame(f_frame),
		m_track(false),
		m_changed(false)
	{}
	void setFrame(QFrame* f_frame) { m_frame = f_frame; }

	void track(bool f_track, bool f_changed);
	bool isChanged() const
	{
		// Control must be in the tracked state to guarantee valid result
		ASSERT(m_track);
		return m_changed;
	}

public slots:
	void onChange();

private:
	ChangeHighlighter();

private:
	QFrame* m_frame;

	bool m_track;
	bool m_changed;
};


class TextEdit : public QTextEdit
{
	Q_OBJECT

public:
	explicit TextEdit(QWidget* parent = 0);

	void trackChanges(bool f_track, bool f_changed = false) { m_tracker.track(f_track, f_changed); }
	bool isChanged() const { return m_tracker.isChanged(); }

private:
	ChangeHighlighter m_tracker;
};


class GenreBox : public QComboBox
{
	Q_OBJECT

public:
	explicit GenreBox(QWidget* parent = 0);
	void setFrame(QFrame* f_frame) { m_tracker.setFrame(f_frame); }
	void setLabel(QLabel* f_label) { m_label = f_label; }

	void trackChanges(bool f_track, bool f_changed = false) { m_tracker.track(f_track, f_changed); }
	bool isChanged() const { return m_tracker.isChanged(); }

private slots:
	void onTextChange(const QString& str);
	void onSelectionChange(int f_index);

private:
	ChangeHighlighter m_tracker;
	QLabel* m_label;
};

#endif //__TEXTEDIT_H__
