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
#include <QGraphicsPixmapItem>


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
bool CJobSingle::save() const
{
	TRACE("Job: save");
	return true;
}


void CJobSingle::trackTag1UI(Ui::Window& f_ui) const
{
	f_ui.editTrack		->trackChanges(true);
	f_ui.editYear		->trackChanges(true);
	f_ui.editTitle		->trackChanges(true);
	f_ui.editArtist		->trackChanges(true);
	f_ui.editAlbum		->trackChanges(true);
	f_ui.editComment	->trackChanges(true);
	f_ui.comboGenre		->trackChanges(true);
}

void CJobSingle::updateTag1UI(Ui::Window& f_ui) const
{
	TRACE("Job: update ID3v1 UI");

	if(const CID3v1* pTag = m_mp3.tagV1())
	{
		uint uTrack = pTag->isV11() ? pTag->getTrack() : 0;
		f_ui.editTrack->setText(uTrack ? QString::number(uTrack) : QString());

		uint uYear = pTag->getYear();
		f_ui.editYear->setText(uYear ? QString::number(uYear) : QString());

		f_ui.editTitle	->setText( QString(pTag->getTitle())	);
		f_ui.editArtist	->setText( QString(pTag->getArtist())	);
		f_ui.editAlbum	->setText( QString(pTag->getAlbum())	);
		f_ui.editComment->setText( QString(pTag->getComment())	);

		f_ui.comboGenre->setCurrentIndex( pTag->getGenreIndex() );
	}

	trackTag1UI(f_ui);
}


void CJobSingle::trackTag2UI(Ui::Window& f_ui) const
{
	f_ui.editDisc		->trackChanges(true);
	f_ui.editBPM		->trackChanges(true);

	trackTag1UI(f_ui);

	f_ui.editAArtist	->trackChanges(true);

	f_ui.editComposer	->trackChanges(true);
	f_ui.editPublisher	->trackChanges(true);
	f_ui.editOrigArtist	->trackChanges(true);
	f_ui.editCopyright	->trackChanges(true);
	f_ui.editURL		->trackChanges(true);
	f_ui.editEncoded	->trackChanges(true);
}

void CJobSingle::updateTag2UI(Ui::Window& f_ui) const
{
	TRACE("Job: update ID3v2 UI");

	if(const CID3v2* pTag = m_mp3.tagV2())
	{
		f_ui.labelTagOffset->setText( QString("@ %1 bytes").arg(m_mp3.tag2Offset()) );

		f_ui.editTrack		->setText( QString::fromStdString(pTag->getTrack())			);
		f_ui.editDisc		->setText( QString::fromStdString(pTag->getDisc())			);
		f_ui.editBPM		->setText( QString::fromStdString(pTag->getBPM())			);

		f_ui.editTitle		->setText( QString::fromStdString(pTag->getTitle())			);
		f_ui.editArtist		->setText( QString::fromStdString(pTag->getArtist())		);
		f_ui.editAlbum		->setText( QString::fromStdString(pTag->getAlbum())			);
		f_ui.editYear		->setText( QString::fromStdString(pTag->getYear())			);
		f_ui.editAArtist	->setText( QString::fromStdString(pTag->getAlbumArtist())	);
		f_ui.editComment	->setText( QString::fromStdString(pTag->getComment())		);

		if(pTag->isExtendedGenre())
			ASSERT(!"Extended Genre");
		int i = pTag->getGenreIndex();
		if(i == -1)
			f_ui.comboGenre->setCurrentText( QString::fromStdString(pTag->getGenre()) );
		else
			f_ui.comboGenre->setCurrentIndex(i);

		f_ui.editComposer	->setText( QString::fromStdString(pTag->getComposer())		);
		f_ui.editPublisher	->setText( QString::fromStdString(pTag->getPublisher())		);
		f_ui.editOrigArtist	->setText( QString::fromStdString(pTag->getOrigArtist())	);
		f_ui.editCopyright	->setText( QString::fromStdString(pTag->getCopyright())		);
		f_ui.editURL		->setText( QString::fromStdString(pTag->getURL())			);
		f_ui.editEncoded	->setText( QString::fromStdString(pTag->getEncoded())		);

		// Image
		const std::vector<uchar>& image = pTag->getPictureData();
		if(image.empty())
			return;

		QGraphicsScene* pScene = f_ui.graphArt->scene();
		ASSERT(pScene);

		QPixmap px;
		if( !px.loadFromData(&image[0], image.size()) )
		{
			TRACE("ERROR: failed to load the image data");
			return;
		}
		ASSERT(!px.isNull());

		int cx = f_ui.graphArt->childrenRect().width();
		int cy = f_ui.graphArt->childrenRect().height();
		if(px.width() > px.height())
		{
			TRACE(QString("Job: scale image %1x%2 -> %3x%4 by width").
				  arg(px.width()).arg(px.height()).
				  arg(cx).arg(cy));
			px = px.scaledToWidth(cx, Qt::SmoothTransformation);
		}
		else
		{
			TRACE(QString("Job: scale image %1x%2 -> %3x%4 by height").
				  arg(px.width()).arg(px.height()).
				  arg(cx).arg(cy));
			px = px.scaledToHeight(cy, Qt::SmoothTransformation);
		}
		ASSERT(!px.isNull());

		ASSERT(pScene->items().isEmpty());
		pScene->addItem(new QGraphicsPixmapItem(px));

		// Unknown Frames
		QStandardItemModel* pModel = static_cast<QStandardItemModel*>(f_ui.listFrames->model());
		ASSERT(pModel);
		for(auto str: pTag->getUnknownFrames())
			pModel->appendRow(new QStandardItem(QString::fromStdString(str)));
	}

	trackTag2UI(f_ui);
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
}
