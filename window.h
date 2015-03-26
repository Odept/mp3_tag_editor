#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <QMainWindow>


namespace Ui
{
	class Window;
	class TextEdit;
}
class CJob;


class Window : public QMainWindow
{
	Q_OBJECT

public:
	explicit Window(QWidget *parent = 0);
	~Window();

protected:
	void dragEnterEvent(QDragEnterEvent* pEvent);
	void dropEvent(QDropEvent* pEvent);

private:
	Ui::Window *ui;

	CJob* m_job;
};

#endif // __WINDOW_H__
