#include "window.h"
#include "ui_window.h"

#include "job_file.h"

#include "External/inc/id3v1.h"
#include "External/inc/id3v2.h"

#include <QFileInfo>
#include <QMessageBox>


bool CJob::init(QWidget& f_parent)
{
	QFileInfo fi(m_file);
	if(!fi.exists())
	{
		QString msg = QString("File \"%1\" doesn't exist").arg(m_file);
		QMessageBox::critical(&f_parent, QString("File open error"), msg);
		return false;
	}

	QFile file(m_file);
	if( !file.open(QIODevice::ReadOnly) )
	{
		QString msg = QString("Failed to open \"%1\" (%2)").arg(m_file).arg(file.error());
		QMessageBox::critical(&f_parent, QString("File open error"), msg);
		return false;
	}

	const uchar* pData = file.map(0, file.size());
	file.close();
	if(!pData)
	{
		QString msg = QString("Failed to map \"%1\"").arg(m_file);
		QMessageBox::critical(&f_parent, QString("File open error"), msg);
		return false;
	}

	m_tag  = QSharedPointer<CID3v1>(CID3v1::gen(pData, file.size()));
	m_tag2 = QSharedPointer<CID3v2>(CID3v2::gen(pData, file.size()));

	return true;
}


CJob::~CJob() {}

// ============================================================================
CJobSingle* CJobSingle::create(Ui::Window& f_ui, const QString& f_file)
{
	//qDebug() << QString("Single mode: %1").arg(f_file);
	CJobSingle* pJob = new CJobSingle(f_ui, f_file);

	if( !CJob::init(*f_ui.centralWidget->parentWidget(), pJob) )
	{
		delete pJob;
		return NULL;
	}

	// Init fields
	pJob->updateUI();

	return pJob;
}


void CJobSingle::updateUI() const
{
	if(m_ui.comboTag->currentTagVersion() == 1 && !m_tag.isNull())
	{
		if(m_tag->isV11())
			m_ui.editTrack->setText( QString::number(m_tag->getTrack()) );
		m_ui.editTitle->setText( m_tag->getTitle() );
		m_ui.editArtist->setText( m_tag->getArtist() );
		m_ui.editAlbum->setText( m_tag->getAlbum() );
		m_ui.editYear->setText( QString::number(m_tag->getYear()) );
		//m_ui.comboGenre->setItemData( Genre->setText ( m_tag->getGenre()  );
		m_ui.editComment->setText( m_tag->getComment() );
	}
	else if(m_ui.comboTag->currentTagVersion() != 1 && !m_tag2.isNull())
	{
		m_ui.editTrack->setText  ( QString::fromStdString(m_tag2->getTrack())       );
		m_ui.editDisc->setText   ( QString::fromStdString(m_tag2->getDisc())        );
		m_ui.editBPM->setText    ( QString::fromStdString(m_tag2->getBPM())         );

		m_ui.editTitle->setText  ( QString::fromStdString(m_tag2->getTitle())       );
		m_ui.editArtist->setText ( QString::fromStdString(m_tag2->getArtist())      );
		m_ui.editAlbum->setText  ( QString::fromStdString(m_tag2->getAlbum())       );
		m_ui.editYear->setText   ( QString::fromStdString(m_tag2->getYear())        );
		m_ui.editAArtist->setText( QString::fromStdString(m_tag2->getAlbumArtist()) );

		//bool				isExtendedGenre() )
		//m_ui.comboGenre->setText( getGenre()
		//const std::string&	getGenreEx()
		//int					getGenreIndex()

		//m_ui.editComment->setText( m_tag2->getComment() );

		m_ui.editComposer->setText ( QString::fromStdString(m_tag2->getComposer())  );
		m_ui.editPublisher->setText( QString::fromStdString(m_tag2->getPublisher()) );
		//m_ui.editOrigArtist->setText( m_tag2->getOArtist() );
		//m_ui.editCopyright->setText( getCopyright()
		//const std::string&	getURL()
		//m_ui.editEncoded->setText( getEncoded()
	}
}
