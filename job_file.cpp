#include "window.h"
#include "ui_window.h"

#include "job_file.h"
#include "mp3.h"

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

	m_mp3 = QSharedPointer<CMP3>(CMP3::gen(pData, file.size()));
	if(m_mp3.isNull())
	{
		QString msg = QString("Failed to parse \"%1\"").arg(m_file);
		QMessageBox::critical(&f_parent, QString("MP3 parse error"), msg);
		return false;
	}

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
	CID3v1& tag = m_mp3->tagV1();

	uint uTrack = tag.isV11() ? tag.getTrack() : 0;
	updateControl(*f_ui.editTrack, uTrack ? QString::number(uTrack) : QString(""));

	uint uYear = tag.getYear();
	updateControl(*f_ui.editYear, uYear ? QString::number(uYear) : QString(""));

	updateControl(*f_ui.editTitle  , QString(tag.getTitle  ()));
	updateControl(*f_ui.editArtist , QString(tag.getArtist ()));
	updateControl(*f_ui.editAlbum  , QString(tag.getAlbum  ()));
	updateControl(*f_ui.editComment, QString(tag.getComment()));

	f_ui.comboGenre->setCurrentIndex( tag.getGenreIndex() );
	f_ui.comboGenre->trackChanges(true);
}

void CJobSingle::updateTag2UI(Ui::Window& f_ui) const
{
	CID3v2& tag = m_mp3->tagV2();

	updateControl(*f_ui.editTrack, tag.getTrack());
	updateControl(*f_ui.editDisc , tag.getDisc() );
	updateControl(*f_ui.editBPM  , tag.getBPM()  );

	updateControl(*f_ui.editTitle  , tag.getTitle()      );
	updateControl(*f_ui.editArtist , tag.getArtist()     );
	updateControl(*f_ui.editAlbum  , tag.getAlbum()      );
	updateControl(*f_ui.editYear   , tag.getYear()       );
	updateControl(*f_ui.editAArtist, tag.getAlbumArtist());
	updateControl(*f_ui.editComment, tag.getComment()    );

	if(tag.isExtendedGenre())
		abort();
	f_ui.comboGenre->setCurrentText( QString::fromStdString(tag.getGenre()) );
	f_ui.comboGenre->trackChanges(true);

	updateControl(*f_ui.editComposer , tag.getComposer());
	updateControl(*f_ui.editPublisher, tag.getPublisher());
	//m_ui.editOrigArtist->setText( QString::fromStdString(tag.getOrigArtist()) );
	//m_ui.editCopyright->setText( QString::fromStdString(tag.getCopyright()) );
	//const std::string&	getURL()
	//m_ui.editEncoded->setText( QString::fromStdString(tag.getEncoded()) );
}
