#include "window.h"
#include "ui_window.h"

#include "job_file.h"

#include "mp3.h"
#include "error.h"

#include "External/inc/id3v1.h"
#include "External/inc/id3v2.h"

#include <QFileInfo>


class EFile : public Error
{
public:
	EFile(const QString& f_msg):
		Error(QString("File open error"), f_msg)
	{}
};

// ============================================================================
CJob::CJob(const QString& f_path):
	m_path(f_path)
{
	QFileInfo fi(m_path);
	if(!fi.exists())
		throw EFile( QString("File \"%1\" doesn't exist").arg(f_path) );

	QFile file(m_path);
	if( !file.open(QIODevice::ReadOnly) )
		throw EFile( QString("Failed to open \"%1\" (%2)").arg(f_path).arg(file.error()) );

	const uchar* pData = file.map(0, file.size());
	file.close();
	if(!pData)
		throw EFile( QString("Failed to map \"%1\"").arg(f_path) );

	m_mp3 = CMP3(pData, file.size());
}

// ============================================================================
void CJobSingle::updateControl(TextEdit& f_control, const QString& f_str) const
{
	f_control.setText(f_str);
	f_control.trackChanges(true);
}

void CJobSingle::updateTag1UI(Ui::Window& f_ui) const
{
	CID3v1& tag = m_mp3.tagV1();

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
	CID3v2& tag = m_mp3.tagV2();

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
