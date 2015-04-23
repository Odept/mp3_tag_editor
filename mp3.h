#ifndef __MP3_H__
#define __MP3_H__

#include "debug.h"

#include <QSharedPointer>


class CID3v1;
class CID3v2;
class CAPE;
class CLyrics;
class CMPEGStream;


class CMP3
{
public:
	CMP3(QWidget* pParent, const uchar* f_data, unsigned long long f_size);
	CMP3() {}
	virtual ~CMP3() {}

	CID3v1* tagV1() const { return m_tag.data();  }
	CID3v2* tagV2() const { return m_tag2.data(); }
	int tag2Offset() const
	{
		ASSERT(!m_tag2.isNull());
		return m_tag2Offset;
	}

	const CMPEGStream& mpeg() const { return *m_mpeg; }
	uint firstFrameOffset() const { return m_mpegOffset; }

private:
	QSharedPointer<CID3v1> m_tag;

	int m_tag2Offset;
	QSharedPointer<CID3v2> m_tag2;

	QSharedPointer<CAPE> m_ape;
	QSharedPointer<CLyrics> m_lyrics;

	uint m_mpegOffset;
	QSharedPointer<CMPEGStream> m_mpeg;
};

#endif // __MP3_H__
