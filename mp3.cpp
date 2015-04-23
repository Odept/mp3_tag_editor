#include "mp3.h"

#include "debug.h"
#include "messagebox.h"

#include "External/inc/id3v1.h"
#include "External/inc/id3v2.h"
#include "External/inc/ape.h"
#include "External/inc/lyrics.h"
#include "External/inc/mpeg.h"


class EMP3 : public Error
{
public:
	EMP3(const QString& f_msg):
		Error(QString("MP3 parse error"), f_msg)
	{}
};


CMP3::CMP3(QWidget* pParent, const uchar* f_data, unsigned long long f_size):
	m_tag2Offset(-1),
	m_mpegOffset(f_size)
{
	TRACE( QString("MP3: init (%1 bytes)").arg(f_size) );

	for(unsigned long long i = 0, size = f_size; i < f_size;)
	{
		const uchar* pData = f_data + i;

		// MPEG stream
		if(m_mpeg.isNull() && CMPEGStream::verifyFrameSequence(pData, size))
		{
			m_mpegOffset = i;
			m_mpeg = QSharedPointer<CMPEGStream>(CMPEGStream::gen(pData, size));

			// Check the last frame for the invalid data
			ASSERT(m_mpeg->getFrameCount());
			uint uLast = m_mpeg->getFrameCount() - 1;
			uint uLastOffset = m_mpegOffset + m_mpeg->getFrameOffset(uLast);

			for(uint o = 0, n = m_mpeg->getFrameSize(uLast); n; o++, n--)
			{
				if( !CAPE::isValidHeader(f_data + uLastOffset + o, n) )
					continue;

				QString msg = QString("APE tag is found in the last MPEG data frame at offset 0x") +
							  QString::number(uLastOffset + o, 16).toUpper() +
							  QString(". The APE tag will be kept, while the frame will be discarded");
				TRACE(QString("WARNING: ") + msg);
				CMessageBox::warning(pParent, "Invalid MP3 layout", msg);

				uint s = m_mpeg->truncate(1);
				ASSERT(s == 1);
				i += o;
				size -= o;
				break;
			}

			TRACE(QString("MP3: MPEG stream @ 0x") +
				  QString::number(m_mpegOffset, 16).toUpper() +
				  QString(" +0x") +
				  QString::number(m_mpeg->getSize(), 16).toUpper());

			i += m_mpeg->getSize();
			size -= m_mpeg->getSize();
			continue;
		}

		// ID3v2
		if(m_tag2.isNull())
		{
			uint uTagSize;
			if(CID3v2* pTag = CID3v2::gen(pData, size, &uTagSize))
			{
				m_tag2Offset = i;
				m_tag2 = QSharedPointer<CID3v2>(pTag);

				TRACE(QString("MP3: ID3v2 @ 0x") +
					  QString::number(m_tag2Offset, 16).toUpper() +
					  QString(" +0x") +
					  QString::number(uTagSize, 16).toUpper());

				i += uTagSize;
				size -= uTagSize;
				continue;
			}
		}

		// ID3v1
		if(m_tag.isNull())
		{
			if(CID3v1* pTag = CID3v1::gen(pData, size))
			{
				if(size != CID3v1::getSize())
				{
					QString msg = QString("ID3v1 tag is found at invalid offset 0x") +
								  QString::number(i, 16).toUpper() +
								  QString(" (0x") +
								  QString::number(f_size - CID3v1::getSize(), 16).toUpper() +
								  QString(" is expected)");
					TRACE(QString("WARNING: ") + msg);
					CMessageBox::warning(pParent, "Invalid MP3 layout", msg);
				}

				m_tag = QSharedPointer<CID3v1>(pTag);

				TRACE(QString("MP3: ID3v1 @ 0x") +
					  QString::number(i, 16).toUpper() +
					  QString(" +0x") +
					  QString::number(m_tag->getSize(), 16).toUpper());

				i += m_tag->getSize();
				size -= m_tag->getSize();
				continue;
			}
		}

		// APE
		if(m_ape.isNull())
		{
			uint uTagSize;
			if(CAPE* pTag = CAPE::gen(pData, size, &uTagSize))
			{
				m_ape = QSharedPointer<CAPE>(pTag);

				TRACE(QString("MP3: APE @ 0x") +
					  QString::number(i, 16).toUpper() +
					  QString(" +0x") +
					  QString::number(uTagSize, 16).toUpper());

				i += uTagSize;
				size -= uTagSize;
				continue;
			}
		}

		// Lyrics
		if(m_lyrics.isNull())
		{
			uint uTagSize;
			if(CLyrics* pTag = CLyrics::gen(pData, size, &uTagSize))
			{
				m_lyrics = QSharedPointer<CLyrics>(pTag);

				TRACE(QString("MP3: Lyrics @ 0x") +
					  QString::number(i, 16).toUpper() +
					  QString(" +0x") +
					  QString::number(uTagSize, 16).toUpper());

				i += uTagSize;
				size -= uTagSize;
				continue;
			}
		}

		// Verify NULLs
		if(f_data[i] != 0)
		{
			if( CMPEGStream::isIncompleteFrame(pData, size) )
			{
				QString msg = QString("Unexpected end of frame at offset 0x") +
							  QString::number(i, 16).toUpper() +
							  QString(". The incomplete frame (%1 bytes) will be discarded").arg(size);
				TRACE(QString("WARNING: ") + msg);
				CMessageBox::warning(pParent, "Invalid MP3 layout", msg);

				//i += size;
				//size = 0;
				break;
			}
			throw EMP3(QString("Unsupported data @ offset %1 (0x").arg(i) +
					   QString::number(i, 16).toUpper() +
					   QString(")"));
		}
		i++;
		size--;
	}

	if(m_mpeg.isNull())
		throw EMP3("Unsupported MP3 file");

/*	if(m_tag.isNull())
	{
		TRACE("MP3: create empty ID3v1 tag");
		m_tag = QSharedPointer<CID3v1>(CID3v1::create());
	}
	if(m_tag2.isNull())
	{
		TRACE("MP3: create empty ID3v2 tag");
		m_tag2 = QSharedPointer<CID3v2>(CID3v2::create());
	}
*/
}
