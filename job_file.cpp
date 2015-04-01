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

	m_tag = QSharedPointer<CID3v1>(CID3v1::gen(pData, file.size()));
	if(m_tag.isNull())
		m_tag = QSharedPointer<CID3v1>(CID3v1::create());
	m_tag2 = QSharedPointer<CID3v2>(CID3v2::gen(pData, file.size()));
	if(m_tag2.isNull())
		m_tag2 = QSharedPointer<CID3v2>(CID3v2::create());

	return true;
}


CJob::~CJob() {}

// ============================================================================
CJobSingle* CJobSingle::create(QWidget& f_parent, const QString& f_file)
{
	CJobSingle* pJob = new CJobSingle(f_file);

	if( pJob->init(f_parent) )
		return pJob;
	else
	{
		delete pJob;
		return NULL;
	}
}


void CJobSingle::updateControl(TextEdit& f_control, const QString& f_str) const
{
	f_control.setText(f_str);
	f_control.trackChanges(true);
}

void CJobSingle::updateTag1UI(Ui::Window& f_ui) const
{
	uint uTrack = m_tag->isV11() ? m_tag->getTrack() : 0;
	updateControl(*f_ui.editTrack, uTrack ? QString::number(uTrack) : QString(""));

	uint uYear = m_tag->getYear();
	updateControl(*f_ui.editYear, uYear ? QString::number(uYear) : QString(""));

	updateControl(*f_ui.editTitle  , QString(m_tag->getTitle  ()));
	updateControl(*f_ui.editArtist , QString(m_tag->getArtist ()));
	updateControl(*f_ui.editAlbum  , QString(m_tag->getAlbum  ()));
	updateControl(*f_ui.editComment, QString(m_tag->getComment()));

	f_ui.comboGenre->setCurrentIndex( m_tag->getGenreIndex() );
	f_ui.comboGenre->trackChanges(true);
}

void CJobSingle::updateTag2UI(Ui::Window& f_ui) const
{
	updateControl(*f_ui.editTrack, m_tag2->getTrack());
	updateControl(*f_ui.editDisc , m_tag2->getDisc() );
	updateControl(*f_ui.editBPM  , m_tag2->getBPM()  );

	updateControl(*f_ui.editTitle  , m_tag2->getTitle()      );
	updateControl(*f_ui.editArtist , m_tag2->getArtist()     );
	updateControl(*f_ui.editAlbum  , m_tag2->getAlbum()      );
	updateControl(*f_ui.editYear   , m_tag2->getYear()       );
	updateControl(*f_ui.editAArtist, m_tag2->getAlbumArtist());
	updateControl(*f_ui.editComment, m_tag2->getComment()    );

	if(m_tag2->isExtendedGenre())
		abort();
	f_ui.comboGenre->setCurrentText(m_tag->getGenre());
	f_ui.comboGenre->trackChanges(true);

	updateControl(*f_ui.editComposer , m_tag2->getComposer());
	updateControl(*f_ui.editPublisher, m_tag2->getPublisher());
	//m_ui.editOrigArtist->setText( QString::fromStdString(m_tag2->getOrigArtist()) );
	//m_ui.editCopyright->setText( QString::fromStdString(m_tag2->getCopyright()) );
	//const std::string&	getURL()
	//m_ui.editEncoded->setText( QString::fromStdString(m_tag2->getEncoded()) );
}
