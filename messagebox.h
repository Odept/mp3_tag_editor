#ifndef __MESSAGE_BOX_H__
#define __MESSAGE_BOX_H__

#include <QMessageBox>


class CMessageBox : public QMessageBox
{
public:
	#define DEF_METHOD(NAME) static StandardButton NAME(QWidget* f_pParent, \
														const QString& f_title, \
														const QString& f_text)
	DEF_METHOD(warning)  { return show( Warning, f_pParent, f_title, f_text); }
	DEF_METHOD(critical) { return show(Critical, f_pParent, f_title, f_text); }
	#undef DEF_METHOD

private:
	static StandardButton show(Icon f_icon, QWidget* f_pParent, const QString& f_title, const QString& f_text)
	{
		QMessageBox mb(f_icon, f_title, f_text, NoButton, f_pParent, Qt::Sheet);
		return (mb.exec() == Accepted) ? (QMessageBox::StandardButton)mb.result() : QMessageBox::NoButton;
	}
};

#endif // __MESSAGE_BOX_H__
