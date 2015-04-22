#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <QMainWindow>
#include <QStandardItemModel>
#include <QGraphicsScene>


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

private:
	void createJob(const QString& f_path);
	bool destroyJob();

	void dragEnterEvent(QDragEnterEvent* pEvent);
	void dropEvent(QDropEvent* pEvent);

	void resetField(TextEdit& f_control, bool f_enabled);
	void resetFields(bool f_enabled = false);
	void resetMPEGInfo();

	void updateMenuAndToolBar();

private slots:
	void onTagSelectionChange(int f_index);

	void on_actionOpen_triggered();

	void on_actionQuit_triggered();

	void on_actionClose_triggered();

private:
	Ui::Window *ui;

	QSharedPointer<CJobSingle> m_job;

	QGraphicsScene m_graphScene;
	QStandardItemModel m_modelFrames;
};

#endif // __WINDOW_H__
