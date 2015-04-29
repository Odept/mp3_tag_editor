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
#define LAMBDA_SYNC(Name) auto sync##Name = [this](const QString& text)
#define SYNC_CALL_TEXTEDIT(Name, Text) \
	syncControl<TextEdit>(*f_ui.edit##Name, Text, #Text, sync##Name)

template<typename T>
void CJobSingle::syncControl(const T& f_control,
							 const QString& f_text,
							 const QString& f_name,
							 std::function<bool (const QString&)> f_lambda)
{
	if( !f_control.isChanged() )
		return;

	TRACE( QString("Sync: %1 \"%2\"").arg(f_name).arg(f_text) );
	if( !f_lambda(f_text) )
		TRACE("WARNING: the value seems to be truncated during set");
}


void CJobSingle::syncTag1UI(Ui::Window& f_ui)
{
	TRACE("Job: sync ID3v1 UI");

	QString track	= f_ui.editTrack	->toPlainText();
	QString title	= f_ui.editTitle	->toPlainText();
	QString artist	= f_ui.editArtist	->toPlainText();
	QString album	= f_ui.editAlbum	->toPlainText();
	QString year	= f_ui.editYear		->toPlainText();
	QString comment	= f_ui.editComment	->toPlainText();
	QString genre	= f_ui.comboGenre	->currentText();

	/**
	 * Remove the ID3v1 tag if all the fields are empty
	 * create it otherwise if necessary
	 */
	if(track	.isEmpty() &&
	   title	.isEmpty() &&
	   artist	.isEmpty() &&
	   album	.isEmpty() &&
	   year		.isEmpty() &&
	   comment	.isEmpty() &&
	   genre	.isEmpty())
	{
		if(m_mp3.tagV1())
			m_mp3.removeTagV1();
		return;
	}
	else if(!m_mp3.tagV1())
		m_mp3.createTagV1();

	// Lambdas
	LAMBDA_SYNC(Track)
	{
		bool ok; int t = text.toInt(&ok);
		return m_mp3.tagV1()->setTrack(ok ? t : (uint)~0);
	};
	LAMBDA_SYNC(  Title)	{ return m_mp3.tagV1()->setTitle  ( text.toLatin1().constData() );	};
	LAMBDA_SYNC( Artist)	{ return m_mp3.tagV1()->setArtist ( text.toLatin1().constData() );	};
	LAMBDA_SYNC(  Album)	{ return m_mp3.tagV1()->setAlbum  ( text.toLatin1().constData() );	};
	LAMBDA_SYNC(   Year)	{ return m_mp3.tagV1()->setYear   ( text.toLatin1().constData() );	};
	LAMBDA_SYNC(Comment)	{ return m_mp3.tagV1()->setComment( text.toLatin1().constData() );	};
	LAMBDA_SYNC(  Genre)
	{
		int i = text.toInt();
		return m_mp3.tagV1()->setGenreIndex((i < 0) ? (uint)~0 : i);
	};

	// Do sync
	SYNC_CALL_TEXTEDIT(  Track, track	);
	SYNC_CALL_TEXTEDIT(  Title, title	);
	SYNC_CALL_TEXTEDIT( Artist, artist	);
	SYNC_CALL_TEXTEDIT(  Album, album	);
	SYNC_CALL_TEXTEDIT(   Year, year	);
	SYNC_CALL_TEXTEDIT(Comment, comment	);

	// Not efficient but OK for now (int->str here and str->int in the lambda)
	genre = QString::number( f_ui.comboGenre->findText(genre) );
	syncControl<GenreBox>(*f_ui.comboGenre , genre, "genre", syncGenre);
}

void CJobSingle::syncTag2UI(Ui::Window& f_ui)
{
	TRACE("Job: sync ID3v2 UI");

	QString track		= f_ui.editTrack		->toPlainText();
	QString disc		= f_ui.editDisc			->toPlainText();
	QString bpm			= f_ui.editBPM			->toPlainText();

	QString title		= f_ui.editTitle		->toPlainText();
	QString artist		= f_ui.editArtist		->toPlainText();
	QString album		= f_ui.editAlbum		->toPlainText();
	QString year		= f_ui.editYear			->toPlainText();
	QString aartist		= f_ui.editAlbumArtist	->toPlainText();
	QString comment		= f_ui.editComment		->toPlainText();
	QString genre		= f_ui.comboGenre		->currentText();

	QString composer	= f_ui.editComposer		->toPlainText();
	QString publisher	= f_ui.editPublisher	->toPlainText();
	QString oartist		= f_ui.editOrigArtist	->toPlainText();
	QString copyright	= f_ui.editCopyright	->toPlainText();
	QString url			= f_ui.editURL			->toPlainText();
	QString encoded		= f_ui.editEncoded		->toPlainText();

	if(track	.isEmpty() &&
	   disc		.isEmpty() &&
	   bpm		.isEmpty() &&

	   title	.isEmpty() &&
	   artist	.isEmpty() &&
	   album	.isEmpty() &&
	   aartist	.isEmpty() &&
	   year		.isEmpty() &&
	   comment	.isEmpty() &&
	   genre	.isEmpty() &&

	   composer	.isEmpty() &&
	   publisher.isEmpty() &&
	   oartist	.isEmpty() &&
	   copyright.isEmpty() &&
	   url		.isEmpty() &&
	   encoded	.isEmpty())
	{
		//if(m_mp3.tagV1())
		//	m_mp3.removeTagV1();
		//return;
	}
	else if(!m_mp3.tagV2())
		m_mp3.createTagV2();

	// Lambdas
	LAMBDA_SYNC(      Track)	{ return m_mp3.tagV2()->setTrack		( text.toStdString() );	};
	LAMBDA_SYNC(       Disc)	{ return m_mp3.tagV2()->setDisc			( text.toStdString() );	};
	LAMBDA_SYNC(        BPM)	{ return m_mp3.tagV2()->setBPM			( text.toStdString() );	};

	LAMBDA_SYNC(      Title)	{ return m_mp3.tagV2()->setTitle		( text.toStdString() );	};
	LAMBDA_SYNC(     Artist)	{ return m_mp3.tagV2()->setArtist		( text.toStdString() );	};
	LAMBDA_SYNC(      Album)	{ return m_mp3.tagV2()->setAlbum		( text.toStdString() );	};
	LAMBDA_SYNC(AlbumArtist)	{ return m_mp3.tagV2()->setAlbumArtist	( text.toStdString() );	};
	LAMBDA_SYNC(       Year)	{ return m_mp3.tagV2()->setYear			( text.toStdString() );	};
	LAMBDA_SYNC(    Comment)	{ return m_mp3.tagV2()->setComment		( text.toStdString() );	};
	LAMBDA_SYNC(      Genre)	{ return m_mp3.tagV2()->setGenre		( text.toStdString() );	};

	LAMBDA_SYNC(   Composer)	{ return m_mp3.tagV2()->setComposer		( text.toStdString() );	};
	LAMBDA_SYNC(  Publisher)	{ return m_mp3.tagV2()->setPublisher	( text.toStdString() );	};
	LAMBDA_SYNC( OrigArtist)	{ return m_mp3.tagV2()->setOrigArtist	( text.toStdString() );	};
	LAMBDA_SYNC(  Copyright)	{ return m_mp3.tagV2()->setCopyright	( text.toStdString() );	};
	LAMBDA_SYNC(        URL)	{ return m_mp3.tagV2()->setURL			( text.toStdString() );	};
	LAMBDA_SYNC(    Encoded)	{ return m_mp3.tagV2()->setEncoded		( text.toStdString() );	};

	// Do sync
	SYNC_CALL_TEXTEDIT(      Track, track		);
	SYNC_CALL_TEXTEDIT(       Disc, disc		);
	SYNC_CALL_TEXTEDIT(        BPM, bpm			);

	SYNC_CALL_TEXTEDIT(      Title, title		);
	SYNC_CALL_TEXTEDIT(     Artist, artist		);
	SYNC_CALL_TEXTEDIT(      Album, album		);
	SYNC_CALL_TEXTEDIT(AlbumArtist, aartist		);
	SYNC_CALL_TEXTEDIT(       Year, year		);
	SYNC_CALL_TEXTEDIT(    Comment, comment		);

	syncControl<GenreBox>(*f_ui.comboGenre, genre, "genre", syncGenre);

	SYNC_CALL_TEXTEDIT(   Composer, composer	);
	SYNC_CALL_TEXTEDIT(  Publisher, publisher	);
	SYNC_CALL_TEXTEDIT( OrigArtist, oartist		);
	SYNC_CALL_TEXTEDIT(  Copyright, copyright	);
	SYNC_CALL_TEXTEDIT(        URL, url			);
	SYNC_CALL_TEXTEDIT(    Encoded, encoded		);

	// Image
	//const std::vector<uchar>& image = pTag->getPictureData();
}

void CJobSingle::sync(Ui::Window& f_ui, uint f_uTag)
{
	if(!f_uTag || f_uTag == 1)
		syncTag1UI(f_ui);
	if(!f_uTag || f_uTag == 2)
		syncTag2UI(f_ui);
}

bool CJobSingle::save(Ui::Window& f_ui)
{
	TRACE("Job: save");
	sync(f_ui, f_ui.comboTag->currentIndex());
	//mp3
	return true;
}

// ====================================
template<typename T>
static void updateControl(TextEdit& f_control,
						  const T& f_tag,
						  std::function<const QString (const T&)> f_lambdaGetText,
						  std::function<bool (const T&)> f_lambdaIsModified)
{
	f_control.setText( f_lambdaGetText(f_tag) );
	f_control.trackChanges(true, f_lambdaIsModified(f_tag));
}

#define LAMBDA_NAME_GET_TEXT(Name)		getText##Name
#define LAMBDA_NAME_IS_MODIFIED(Name)	isModified##Name

#define LAMBDA_IS_MODIFIED(Name, Type) \
	auto LAMBDA_NAME_IS_MODIFIED(Name) = [](const Type& tag) \
	{ \
		return tag.isModified##Name(); \
	}

#define UPDATE_CONTROL_RAW(Name, Type) \
	updateControl<Type>(*f_ui.edit##Name, *pTag, LAMBDA_NAME_GET_TEXT(Name), LAMBDA_NAME_IS_MODIFIED(Name))

#define DEF_UPDATE_CONTROL(Name, Type) \
	LAMBDA_GET_TEXT(Name); \
	LAMBDA_IS_MODIFIED(Name, Type); \
	UPDATE_CONTROL_RAW(Name, Type)


void CJobSingle::updateTag1UI(Ui::Window& f_ui) const
{
	TRACE("Job: update ID3v1 UI");

	const CID3v1* pTag = m_mp3.tagV1();
	if(!pTag)
		return;

	f_ui.labelTagInfo->setText( QString("version 1%1").arg(pTag->isV11() ? ".1" : "") );

	// Do update
#define LAMBDA_GET_TEXT(Name) \
	auto LAMBDA_NAME_GET_TEXT(Name) = [](const CID3v1& tag) \
	{ \
		return QString(tag.get##Name()); \
	}
#define UPDATE_CONTROL(Name) DEF_UPDATE_CONTROL(Name, CID3v1)

	auto LAMBDA_NAME_GET_TEXT(Track) = [](const CID3v1& tag)
	{
		uint uTrack = tag.isV11() ? tag.getTrack() : 0;
		return uTrack ? QString::number(uTrack) : QString();
	};
	LAMBDA_IS_MODIFIED(Track, CID3v1);

	UPDATE_CONTROL_RAW	(  Track, CID3v1);
	UPDATE_CONTROL		(   Year);
	UPDATE_CONTROL		(  Title);
	UPDATE_CONTROL		( Artist);
	UPDATE_CONTROL		(  Album);
	UPDATE_CONTROL		(Comment);

	f_ui.comboGenre->setCurrentIndex( pTag->getGenreIndex() );
	f_ui.comboGenre->trackChanges(true, pTag->isModifiedGenre());
#undef UPDATE_CONTROL
#undef LAMBDA_GET_TEXT
}


void CJobSingle::updateTag2UI(Ui::Window& f_ui) const
{
	TRACE("Job: update ID3v2 UI");

	const CID3v2* pTag = m_mp3.tagV2();
	if(!pTag)
		return;

	f_ui.labelTagInfo->setText( QString("@ %1 bytes").arg(m_mp3.tag2Offset()) );

	// Do update
#define LAMBDA_GET_TEXT(Name) \
	auto LAMBDA_NAME_GET_TEXT(Name) = [](const CID3v2& tag) \
	{ \
		return QString::fromStdString(tag.get##Name()); \
	}
#define UPDATE_CONTROL(Name) DEF_UPDATE_CONTROL(Name, CID3v2)

	UPDATE_CONTROL(Track);
	UPDATE_CONTROL(Disc);
	UPDATE_CONTROL(BPM);

	UPDATE_CONTROL(Title);
	UPDATE_CONTROL(Artist);
	UPDATE_CONTROL(Album);
	UPDATE_CONTROL(Year);
	UPDATE_CONTROL(AlbumArtist);
	UPDATE_CONTROL(Comment);

	std::string genre = pTag->getGenre();
	ASSERT(!pTag->isExtendedGenre());
	int i = CID3v2::genre(genre);
	if(i == -1)
		f_ui.comboGenre->setCurrentText( QString::fromStdString(genre) );
	else
	{
		f_ui.comboGenre->setCurrentIndex(i);
		//f_ui.comboGenre->emit currentIndexChanged(i);
	}
	f_ui.comboGenre->trackChanges(true, pTag->isModifiedGenre());

	UPDATE_CONTROL(Composer);
	UPDATE_CONTROL(Publisher);
	UPDATE_CONTROL(OrigArtist);
	UPDATE_CONTROL(Copyright);
	UPDATE_CONTROL(URL);
	UPDATE_CONTROL(Encoded);

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
#undef UPDATE_CONTROL
#undef LAMBDA_GET_TEXT
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
