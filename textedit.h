#ifndef __TEXTEDIT_H__
#define __TEXTEDIT_H__

#include <QTextEdit>
#include <QComboBox>
#include <QLabel>


class TextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit TextEdit(QWidget* parent = 0):
        QTextEdit(parent)
    {
        connect((QTextEdit*)this, SIGNAL(textChanged()), this, SLOT(onTextChange()));
    }

private slots:
    void onTextChange();
};


class GenreBox : public QComboBox
{
    Q_OBJECT

public:
    explicit GenreBox(QWidget* parent = 0):
        QComboBox(parent)
    {
        connect((QComboBox*)this, SIGNAL(editTextChanged(const QString&)),
                            this, SLOT  (onTextChange   (const QString&)));
    }

    void connectLabel(QLabel* label) { m_label = QSharedPointer<QLabel>(label); }

private slots:
    void onTextChange(const QString& str);

private:
    QSharedPointer<QLabel> m_label;
};

#endif //__TEXTEDIT_H__
