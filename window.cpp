#include "window.h"
#include "ui_window.h"
#include "job_file.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDebug>

#include "External/inc/genre.h"


Window::Window(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Window),
	m_job(NULL)
{
	ui->setupUi(this);

	ui->comboGenre->setLabel(ui->labelBoxGenre);
	for(unsigned int i = 0;; i++)
	{
		const char* szGenre = CGenre::get(i);
		if(!*szGenre)
			break;
		QString genre(szGenre);
		//qDebug() << QString("%1: %2").arg(i).arg(genre);
		ui->comboGenre->addItem(genre);
	}

	QStringList tags( QList<QString>() << "Combined" << "ID3v1" << "ID3v2" );
	ui->comboTag->addItems(tags);
	connect(ui->comboTag, SIGNAL(currentIndexChanged (int)),
					this, SLOT  (onTagSelectionChange(int)));

	resetFields();
}

Window::~Window()
{
	delete m_job;
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
	if(m_job)
	{
		//if( !question )
		//	return;
		delete m_job;
		m_job = NULL;

		resetFields();
	}

	const QList<QUrl> urls = pEvent->mimeData()->urls();
	const int nURLs = urls.size();
	if(nURLs == 1)
	{
		ui->tabsMode->setCurrentIndex(0);
		QFileInfo fi(urls[0].toLocalFile());
		m_job = CJobSingle::create(*this, fi.absoluteFilePath());
		if(m_job)
		{
			resetFields(true);
			onTagSelectionChange(ui->comboTag->currentIndex());
		}
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
		//  m_job = QJobBatch(*this, files);
	}
}


void Window::resetField(TextEdit& f_control, bool f_enabled)
{
	f_control.setEnabled(f_enabled);
	f_control.trackChanges(false);
	f_control.clear();
}

void Window::resetFields(bool f_enabled)
{
	ui->comboTag->setEnabled(f_enabled);

	// Clear ID3v1 fields
	resetField(*ui->editTrack  , f_enabled);
	resetField(*ui->editTitle  , f_enabled);
	resetField(*ui->editArtist , f_enabled);
	resetField(*ui->editAlbum  , f_enabled);
	resetField(*ui->editYear   , f_enabled);
	resetField(*ui->editComment, f_enabled);

	ui->comboGenre->setEnabled(f_enabled);
	//ui->comboGenre->clear();

	// Clear ID3v2 fields
	resetField(*ui->editDisc     , f_enabled);
	resetField(*ui->editBPM      , f_enabled);
	resetField(*ui->editAArtist  , f_enabled);
	resetField(*ui->editComposer , f_enabled);
	resetField(*ui->editPublisher, f_enabled);
}


void Window::onTagSelectionChange(int f_index)
{
	bool isTabV2 = (f_index != 1);

	ui->comboGenre->setEditable(isTabV2);

	ui->editDisc->setVisible(isTabV2);
	ui->labelDisc->setVisible(isTabV2);

	ui->editBPM->setVisible(isTabV2);
	ui->labelBPM->setVisible(isTabV2);

	ui->editAArtist->setVisible(isTabV2);
	ui->labelAArtist->setVisible(isTabV2);

	ui->editComposer->setVisible(isTabV2);
	ui->labelComposer->setVisible(isTabV2);

	ui->editPublisher->setVisible(isTabV2);
	ui->labelPublisher->setVisible(isTabV2);

	if(m_job)
	{
		resetFields(true);

		if(isTabV2)
			m_job->updateTag2UI(*ui);
		else
			m_job->updateTag1UI(*ui);
	}
}
