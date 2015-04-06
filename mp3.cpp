#include "mp3.h"

#include "error.h"

#include "External/inc/id3v1.h"
#include "External/inc/id3v2.h"
#include "External/inc/mpeg.h"


class EMP3 : public Error
{
public:
	EMP3(const QString& f_msg):
		Error(QString("MP3 parse error"), f_msg)
	{}
};


CMP3::CMP3(const uchar* f_data, unsigned long long f_size):
	m_tag2Offset(-1),
	m_mpegOffset(f_size)
{
	for(unsigned long long i = 0, size = f_size; i < f_size;)
	{
		const uchar* pData = f_data + i;

		if(m_mpeg.isNull() && CMPEGStream::verifyFrameSequence(pData, size))
		{
			m_mpegOffset = i;
			m_mpeg = QSharedPointer<CMPEGStream>(CMPEGStream::gen(pData, size));
			i += m_mpeg->getSize();
			size -= m_mpeg->getSize();
			continue;
		}
		if(m_tag2.isNull())
		{
			uint uTagSize;
			CID3v2* pTag = CID3v2::gen(pData, size, &uTagSize);
			if(pTag)
			{
				m_tag2Offset = i;
				m_tag2 = QSharedPointer<CID3v2>(pTag);
				i += uTagSize;
				size -= uTagSize;
				continue;
			}
		}
		if(size == CID3v1::getSize())
		{
			m_tag = QSharedPointer<CID3v1>(CID3v1::gen(pData, size));
			i += CID3v1::getSize();
			size -= CID3v1::getSize();
		}

		if(f_data[i] != 0)
			throw EMP3(QString("Unsupported data @ offset %1").arg(i));
		i++;
		size--;
	}

	if(m_mpeg.isNull())
		throw EMP3("Unsupported MP3 file");

	if(m_tag.isNull())
		m_tag = QSharedPointer<CID3v1>(CID3v1::create());
	if(m_tag2.isNull())
		m_tag2 = QSharedPointer<CID3v2>(CID3v2::create());
}
