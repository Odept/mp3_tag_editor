#ifndef __MP3_H__
#define __MP3_H__

#include <QSharedPointer>


class CID3v1;
class CID3v2;
class CMPEGStream;


class CMP3
{
public:
	CMP3(const uchar* f_data, unsigned long long f_size);
	CMP3() {}
	virtual ~CMP3() {}

	CID3v1& tagV1() const { return *m_tag;  }
	CID3v2& tagV2() const { return *m_tag2; }

	uint firstFrameOffset() const { return m_mpegOffset; }
	const CMPEGStream& mpeg() const { return *m_mpeg; }

private:
	QSharedPointer<CID3v1> m_tag;
	QSharedPointer<CID3v2> m_tag2;

	uint m_mpegOffset;
	QSharedPointer<CMPEGStream> m_mpeg;
};

#endif // __MP3_H__
