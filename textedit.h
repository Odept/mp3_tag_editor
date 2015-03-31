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
	void setLabel(QLabel* f_label) { m_label = f_label; }

private slots:
    void onTextChange(const QString& str);
	void onSelectionChange(int f_index);

private:
	QLabel* m_label;
};

#endif //__TEXTEDIT_H__
