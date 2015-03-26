#include "window.h"
#include "ui_window.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDebug>

#include "External/inc/genre.h"


Window::Window(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Window)
{
	ui->setupUi(this);

	QStringList tags( QList<QString>() << "All" << "ID3v1" << "ID3v2" );
	ui->comboTag->addItems(tags);

	ui->comboGenre->connectLabel(ui->labelBoxGenre);
	for(unsigned int i = 0;; i++)
	{
		const char* szGenre = CGenre::get(i);
		if(!*szGenre)
			break;
		QString genre(szGenre);
		ui->comboGenre->addItem(genre);
	}
}

Window::~Window()
{
	delete ui;
}


void Window::dragEnterEvent(QDragEnterEvent* pEvent)
{
	const QMimeData* pMime = pEvent->mimeData();
	if(!pMime->hasUrls())
		return;

	const QList<QUrl> urls = pMime->urls();
	for(int i = 0; i < urls.size(); i++)
	{
		const QUrl& url = urls[i];
		if(!url.isLocalFile())
			continue;
		QFileInfo fi(url.fileName());
		if(!fi.isDir() && (fi.suffix() == QString("mp3")))
		{
			pEvent->acceptProposedAction();
			return;
		}
	}
}

void Window::dropEvent(QDropEvent* pEvent)
{
	/*if(m_job && !m_job.close())
		return;
	*/
	const QList<QUrl> urls = pEvent->mimeData()->urls();
	const int nURLs = urls.size();
	if(nURLs == 1)
	{
		ui->tabsMode->setCurrentIndex(0);
		QFileInfo fi(urls[0].toLocalFile());
		//qDebug() << QString("Single mode: %1").arg(fi.absoluteFilePath());
		//m_job = QJobSingle(fi.absoluteFilePath());
	}
	else
	{
		QStringList files;
		ui->tabsMode->setCurrentIndex(1);
		for(int i = 0; i < nURLs; i++)
		{
			if(!urls[i].isLocalFile())
				continue;
			QFileInfo fi(urls[i].toLocalFile());
			if(fi.isDir() || (fi.suffix() != QString("mp3")))
				continue;
			files.append(fi.absoluteFilePath());
		}
		//if(files.size())
		//  m_job = QJobBatch(files);
	}
}
