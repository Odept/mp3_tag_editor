#include "window.h"
#include "ui_window.h"

#include "job_file.h"
#include "settings.h"
#include "messagebox.h"
#include "debug.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QScrollBar>

#include "External/inc/genre.h"


Window::Window(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Window)
{
	TRACE("Window: create");

	ui->setupUi(this);

	// Tag Selector
	QStringList tags( QList<QString>() << "Combined" << "ID3v1" << "ID3v2" );
	ui->comboTag->addItems(tags);
	connect(ui->comboTag, SIGNAL(currentIndexChanged (int)),
					this, SLOT  (onTagSelectionChange(int)));

	// Genre Controls
	ui->frameGenre->setFrameShape(QFrame::Shape::NoFrame);

	ui->comboGenre->setFrame(ui->frameGenre);
	ui->comboGenre->setLabel(ui->labelBoxGenre);

	for(unsigned int i = 0;; i++)
	{
		const char* szGenre = CGenre::get(i);
		if(!*szGenre)
			break;
		QString genre(szGenre);
		ui->comboGenre->addItem(genre);
	}

	// Album Art
	ui->graphArt->setScene(&m_graphScene);
	connect(ui->graphArt, SIGNAL(clicked()),
					this, SLOT  (onImageClick()));

	// Unknown Frames
	ui->listFrames->setModel(&m_modelFrames);

	// Common
	updateMenuAndToolBar();
	resetFields();
	resetMPEGInfo();

	// Settings
	CSettings s;
	QRect r = s.getWindowRect();

	QRect screen = QApplication::desktop()->screenGeometry();
	if(!r.width() || !r.height())
	{
		uint cx = geometry(). width();
		uint cy = geometry().height();
		uint x = (screen. width() - cx) / 2;
		uint y = (screen.height() - cy) / 2;
		TRACE( QString("Window: set initial pos to center (%1; %2) %3x%4").
			   arg(x).arg(y).arg(cx).arg(cy) );
		move(x, y);
	}
	else
	{
		uint cx = qMin((uint)r. width(), (uint)screen. width());
		uint cy = qMin((uint)r.height(), (uint)screen.height());

		uint x = qMin((uint)screen. width() - cx, (uint)qMax(r.x(), 0));
		uint y = qMin((uint)screen.height() - cy, (uint)qMax(r.y(), 0));

		TRACE( QString("Window: set initial pos to (%1; %2) %3x%4").
					   arg(x).arg(y).arg(cx).arg(cy) );
		resize(cx, cy);
		move(x, y);
	}

	ui->comboTag->setCurrentIndex(0);
	ui->comboTag->emit currentIndexChanged(0);
}

Window::~Window()
{
	TRACE("Window: destroy");

	QRect r(x() + 10, y() + 10, width(), height());
	CSettings().setWindowRect(r);

	delete ui;
}

// ============================================================================
void Window::createJob(const QString& f_path)
{
	TRACE("Window: create job");

	ASSERT(m_job.isNull());
	NEW_EXCEPT(m_job = QSharedPointer<CJobSingle>(new CJobSingle(this, f_path)), this);
	if(m_job.isNull())
	{
		TRACE("Window: cancel create job");
		return;
	}

	ui->editPath->setText(f_path);
	ui->editPath->horizontalScrollBar()->setValue(ui->editPath->horizontalScrollBar()->maximum());

	// Fields are reset and updated by the following call
	ui->comboTag->emit currentIndexChanged(ui->comboTag->currentIndex());

	ui->boxInfo->setVisible(true);
	m_job->updateMPEGInfo(*ui);

	updateMenuAndToolBar();
}

bool Window::destroyJob()
{
	if(m_job.isNull())
		return true;

	TRACE("Window: destroy job");

	//if( !question )
	//	return false;
	m_job.clear();

	ui->editPath->clear();
	updateMenuAndToolBar();
	resetFields();
	resetMPEGInfo();

	return true;
}


void Window::dragEnterEvent(QDragEnterEvent* pEvent)
{
	const QMimeData* pMime = pEvent->mimeData();
	if(!pMime->hasUrls())
	{
		TRACE("Drag: no URLs");
		return;
	}

	const QList<QUrl> urls = pMime->urls();
	TRACE(QString("Drag: %1 URL(s)").arg(urls.size()));

	for(int i = 0; i < urls.size(); i++)
	{
		const QUrl& url = urls[i];
		if(!url.isLocalFile())
		{
			TRACE( QString("Drag: not a local file (%1)").arg(url.fileName()) );
			continue;
		}
		QFileInfo fi(url.fileName());
		if(!fi.isDir() && !fi.suffix().compare("mp3", Qt::CaseInsensitive))
		{
			pEvent->acceptProposedAction();
			return;
		}
		TRACE( QString("Drag: skip non-MP3 file (%1)").arg(url.fileName()) );
	}
}

void Window::dropEvent(QDropEvent* pEvent)
{
	const QList<QUrl> urls = pEvent->mimeData()->urls();
	const int nURLs = urls.size();

	if(nURLs == 1)
	{
		TRACE( QString("Drop: single file (%1)").arg(urls[0].toLocalFile()) );

		if(!destroyJob())
		{
			TRACE("Drop: cancel (active)");
			return;
		}

		ui->tabsMode->setCurrentIndex(0);
		QFileInfo fi(urls[0].toLocalFile());
		createJob(fi.absoluteFilePath());
	}
	else
	{
		TRACE( QString("Drop: %1 files").arg(nURLs) );

		QStringList files;
		ui->tabsMode->setCurrentIndex(1);
		for(int i = 0; i < nURLs; i++)
		{
			if(!urls[i].isLocalFile())
				continue;
			QFileInfo fi(urls[i].toLocalFile());
			if(fi.isDir() || fi.suffix().compare("mp3", Qt::CaseInsensitive))
				continue;
			files.append(fi.absoluteFilePath());
		}
		//if(files.size())
		//  m_job = QJobBatch(*this, files);
	}

	pEvent->acceptProposedAction();
}

// ====================================
void Window::on_actionOpen_triggered()
{
	CSettings s;
	QString dir = s.getOpenDir();

	TRACE("Window(action): open @ " + dir);
	//files = QFileDialog::getOpenFileNames(this, QString("Select music files..."), "",
	//									  QString("MP3 Files (*.mp3);;All Files (*.*)"),
	//									  0, QFileDialog::ReadOnly);
	QFileDialog dialog(this, QString("Select music files..."), dir);
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(tr("MP3 Files (*.mp3);;All Files (*.*)"));
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setOptions(QFileDialog::ReadOnly);

	int res = dialog.exec();
	s.setOpenDir(dialog.directory().absolutePath());
	if(res != QDialog::Accepted)
	{
		TRACE("Window(action): cancel open");
		return;
	}

	QStringList files = dialog.selectedFiles();
	if(files.size() == 1)
	{
		if(!destroyJob())
		{
			TRACE("Window(action): cancel open (active)");
			return;
		}
		createJob(files[0]);
	}
}

void Window::on_actionClose_triggered()
{
	TRACE("Window(action): close");
	if(!destroyJob())
		TRACE("Window(action): cancel close (active)");
}

void Window::on_actionQuit_triggered()
{
	TRACE("Window(action): quit");
	//if(m_job /*|| m_job_batch*/)
	// question
	this->close();
}

void Window::onImageClick()
{
	TRACE("Window: album art clicked");
}

// ============================================================================
void Window::resetField(TextEdit& f_control, bool f_enabled)
{
	f_control.setEnabled(f_enabled);
	f_control.trackChanges(false);
	f_control.clear();
}

void Window::resetFields(bool f_enabled)
{
	TRACE( QString("Window: reset fields (%1)").arg(f_enabled) );

	ui->comboTag->setEnabled(f_enabled);

	// Clear ID3v1 fields
	resetField(*ui->editTrack  , f_enabled);
	resetField(*ui->editTitle  , f_enabled);
	resetField(*ui->editArtist , f_enabled);
	resetField(*ui->editAlbum  , f_enabled);
	resetField(*ui->editYear   , f_enabled);
	resetField(*ui->editComment, f_enabled);

	ui->comboGenre->setEnabled(f_enabled);
	ui->comboGenre->trackChanges(false);
	ui->comboGenre->setCurrentIndex(-1);

	// Clear ID3v2 fields
	ui->labelTagOffset->clear();

	resetField(*ui->editDisc      , f_enabled);
	resetField(*ui->editBPM       , f_enabled);
	resetField(*ui->editAArtist   , f_enabled);
	resetField(*ui->editComposer  , f_enabled);
	resetField(*ui->editPublisher , f_enabled);
	resetField(*ui->editOrigArtist, f_enabled);
	resetField(*ui->editCopyright , f_enabled);
	resetField(*ui->editURL       , f_enabled);
	resetField(*ui->editEncoded   , f_enabled);

	ui->graphArt->setEnabled(f_enabled);
	m_graphScene.clear();

	ui->listFrames->setEnabled(f_enabled);
	m_modelFrames.clear();
}


void Window::resetMPEGInfo()
{
	TRACE("Window: reset MPEG info");

	ui->boxInfo->setVisible(false);
	ui->boxInfo->setTitle( QString("MPEG Info") );

	ui->labelBitrate->clear();
	ui->labelSamplingRate->clear();
	ui->labelMode->clear();
	ui->labelEmphasis->clear();

	ui->labelOffset->clear();
	ui->labelFrames->clear();
	ui->labelLen->clear();
}


void Window::updateMenuAndToolBar()
{
	ui->actionClose		->setEnabled(!m_job.isNull());
	ui->actionSave		->setEnabled(!m_job.isNull());
	ui->actionTagCopy	->setEnabled(!m_job.isNull());
	ui->actionTagPaste	->setEnabled(!m_job.isNull());
}


void Window::onTagSelectionChange(int f_index)
{
	TRACE( QString("Window: tag selection changed to %1").arg(f_index) );

	bool isTabV2 = (f_index != 1);

	ui->labelTagOffset->setVisible(f_index == 2);

	ui->comboGenre->setEditable(isTabV2);

	ui->labelDisc->setVisible(isTabV2);
	ui->editDisc->setVisible(isTabV2);

	ui->labelBPM->setVisible(isTabV2);
	ui->editBPM->setVisible(isTabV2);

	ui->labelAArtist->setVisible(isTabV2);
	ui->editAArtist->setVisible(isTabV2);

	ui->labelComposer->setVisible(isTabV2);
	ui->editComposer->setVisible(isTabV2);

	ui->labelPublisher->setVisible(isTabV2);
	ui->editPublisher->setVisible(isTabV2);

	ui->boxTagEx->setVisible(isTabV2);

	if(m_job.isNull())
		return;

	resetFields(true);
	if(isTabV2)
		m_job->updateTag2UI(*ui);
	else
		m_job->updateTag1UI(*ui);
}
