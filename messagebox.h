#ifndef __MESSAGE_BOX_H__
#define __MESSAGE_BOX_H__

#include <QMessageBox>
#include <QPushButton>


class CMessageBox : public QMessageBox
{
public:
	static const StandardButton TagKeep		= Apply;
	static const StandardButton TagRemove	= Discard;

private:
	struct Button
	{
		QString Text;
		StandardButton Type;
		bool Default;

		Button(const QString& f_text, StandardButton f_type, bool f_default):
			Text(f_text), Type(f_type), Default(f_default)
		{}

	private:
		Button();
	};
	typedef QList<Button> buttons_t;

public:
	#define DEF_METHOD(NAME) static StandardButton NAME(QWidget* f_pParent, \
														const QString& f_title, \
														const QString& f_text)
	DEF_METHOD(warning)  { return show( Warning, f_pParent, f_title, f_text); }
	DEF_METHOD(critical) { return show(Critical, f_pParent, f_title, f_text); }
	DEF_METHOD(questionTag)
	{
		buttons_t buttons;
		buttons.append( Button("Keep"  ,   TagKeep,  true) );
		buttons.append( Button("Remove", TagRemove, false) );
		return show(Question, f_pParent, f_title, f_text, buttons);
	}
	#undef DEF_METHOD

private:
	static StandardButton show(Icon f_icon, QWidget* f_pParent,
							   const QString& f_title, const QString& f_text,
							   const buttons_t& f_buttons = buttons_t())
	{
		QMessageBox mb(f_icon, f_title, f_text, NoButton, f_pParent, Qt::Sheet);

		QPushButton* pDef = NULL;
		for(auto b: f_buttons)
		{
			QPushButton* pB = mb.addButton(b.Type);
			pB->setText(b.Text);
			if(b.Default)
				pDef = pB;
		}
		if(pDef)
			mb.setDefaultButton(pDef);

		return (StandardButton)mb.exec();
	}
};

#endif // __MESSAGE_BOX_H__
