#include "window.h"
#include "ui_window.h"

#include "job_file.h"

#include "mp3.h"
#include "debug.h"

#include "External/inc/id3v1.h"
#include "External/inc/id3v2.h"
#include "External/inc/mpeg.h"

#include <QFileInfo>
#include <QDateTime>
#include <QtMath>


class EFile : public Error
{
public:
	EFile(const QString& f_msg):
		Error(QString("File open error"), f_msg)
	{}
};

// ============================================================================
CJob::CJob(QWidget* pParent, const QString& f_path):
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

	m_mp3 = CMP3(pParent, pData, file.size());
}

// ============================================================================
void CJobSingle::updateControl(TextEdit& f_control, const QString& f_str) const
{
	f_control.setText(f_str);
	f_control.trackChanges(true);
}

void CJobSingle::updateTag1UI(Ui::Window& f_ui) const
{
	TRACE("Job: update ID3v1 UI");

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
	TRACE("Job: update ID3v2 UI");

	CID3v2& tag = m_mp3.tagV2();

	f_ui.labelTagOffset->setText( QString("@ %1 bytes").arg(m_mp3.tag2Offset()) );

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
		ASSERT(!"Extended Genre");
	int i = tag.getGenreIndex();
	if(i == -1)
		f_ui.comboGenre->setCurrentText( QString::fromStdString(tag.getGenre()) );
	else
		f_ui.comboGenre->setCurrentIndex(i);
	f_ui.comboGenre->trackChanges(true);

	updateControl(*f_ui.editComposer  , tag.getComposer()  );
	updateControl(*f_ui.editPublisher , tag.getPublisher() );
	updateControl(*f_ui.editOrigArtist, tag.getOrigArtist());
	updateControl(*f_ui.editCopyright , tag.getCopyright() );
	updateControl(*f_ui.editURL       , tag.getURL()       );
	updateControl(*f_ui.editEncoded   , tag.getEncoded()   );
}

void CJobSingle::updateMPEGInfo(Ui::Window& f_ui) const
{
	TRACE("Job: update MPEG info");

	const CMPEGStream& mpeg = m_mp3.mpeg();

	uint offset = mpeg.getFrameOffset(0);

	f_ui.boxInfo->setTitle( QString("MPEG %1 Layer %2").arg(mpeg.getVersion()).arg(mpeg.getLayer()) );

	f_ui.labelBitrate->setText( QString("%1 kbps %2").arg(mpeg.getBitrate()).arg(mpeg.isVBR() ? " (VBR)" : "") );
	f_ui.labelSamplingRate->setText( QString("%1 Hz").arg(mpeg.getSamplingRate()) );
	f_ui.labelMode->setText( QString(mpeg.getChannelMode()) );
	f_ui.labelEmphasis->setText( QString(mpeg.getEmphasis()) );

	f_ui.labelOffset->setText(QString("%1 (0x").arg(m_mp3.firstFrameOffset()) +
							  QString::number(m_mp3.firstFrameOffset(), 16).toUpper() +
							  QString(") bytes") +
							  QString(offset ? " (+Xing)" : ""));
	f_ui.labelFrames->setText( QString::number(mpeg.getFrameCount()) );

	// Format time
	float fTime = mpeg.getLength();
	QDateTime time( QDateTime::fromTime_t(uint(fTime)).toUTC().addMSecs( uint((fTime - qFloor(fTime)) * 1000) ) );

	QString str;
	if(fTime >= 60 * 60)
		str = time.toString("hh:mm:ss.zzz");
	else if(fTime >= 60)
		str = time.toString("mm:ss.zzz");
	else
		str = time.toString("ss.zzz");
	f_ui.labelLen->setText( time.toString(str) );

	f_ui.boxInfo->setVisible(true);
}
