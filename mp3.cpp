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
	m_mpegOffset( CMPEGStream::calcFirstHeaderOffset(f_data, f_size) )
{
	if(m_mpegOffset >= f_size)
		throw EMP3("Unsupported MP3 file");

	m_tag = QSharedPointer<CID3v1>(CID3v1::gen(f_data, f_size));
	if(m_tag.isNull())
		m_tag = QSharedPointer<CID3v1>(CID3v1::create());
	m_tag2 = QSharedPointer<CID3v2>(CID3v2::gen(f_data, f_size));
	if(m_tag2.isNull())
		m_tag2 = QSharedPointer<CID3v2>(CID3v2::create());

	m_mpeg = QSharedPointer<CMPEGStream>(CMPEGStream::gen(f_data + m_mpegOffset, f_size - m_mpegOffset));
}
