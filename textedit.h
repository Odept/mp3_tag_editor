#ifndef __TEXTEDIT_H__
#define __TEXTEDIT_H__

#include <QTextEdit>
#include <QComboBox>
#include <QLabel>


class TextEdit : public QTextEdit
{
    Q_OBJECT

public:
	explicit TextEdit(QWidget* parent = 0);

	void trackChanges(bool f_track);

private slots:
	void onTextChange();

private:
	QPalette m_palDefault;
	QPalette m_palHightlight;
	bool m_highlight;
};


class GenreBox : public QComboBox
{
    Q_OBJECT

public:
	explicit GenreBox(QWidget* parent = 0);

    void connectLabel(QLabel* label) { m_label = QSharedPointer<QLabel>(label); }

private slots:
    void onTextChange(const QString& str);

private:
    QSharedPointer<QLabel> m_label;
};


class TagBox : public QComboBox
{
	Q_OBJECT

public:
	explicit TagBox(QWidget* parent = 0);

	uint currentTagVersion() const { return currentIndex(); }

//private slots:
//	void onSelectionChange(int f_index) { m_index = f_index; }

private:
	uint m_index;
};

#endif //__TEXTEDIT_H__
