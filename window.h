#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <QMainWindow>


namespace Ui
{
	class Window;
}
class TextEdit;
class CJobSingle;


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
	void resetField(TextEdit& f_control, bool f_enabled);
	void resetFields(bool f_enabled = false);

private slots:
	void onTagSelectionChange(int f_index);

private:
	Ui::Window *ui;

	CJobSingle* m_job;
};

#endif // __WINDOW_H__
