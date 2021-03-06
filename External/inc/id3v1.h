#ifndef __ID3_V1_H__
#define __ID3_V1_H__

#pragma once


struct Tag;


class CID3v1
{
private:
	typedef unsigned int	uint;
	typedef unsigned char	uchar;

public:
	static uint getSize();
	static CID3v1* create();
	static CID3v1* gen(const uchar* f_pData, unsigned long long f_size);

public:
	bool isV11() const;
	//void setV11(bool f_val);

#define DECL_GETTER_SETTER(Type, Name) \
	Type get##Name() const; \
	bool set##Name(Type)
#define DECL_GETTER_SETTER_MODIFIED(Type, Name) \
	DECL_GETTER_SETTER(Type, Name); \
	bool isModified##Name() const

	DECL_GETTER_SETTER_MODIFIED(const char*, Title     );
	DECL_GETTER_SETTER_MODIFIED(const char*, Artist    );
	DECL_GETTER_SETTER_MODIFIED(const char*, Album     );
	DECL_GETTER_SETTER_MODIFIED(const char*, Year      );
	DECL_GETTER_SETTER_MODIFIED(const char*, Comment   );
	DECL_GETTER_SETTER_MODIFIED(      uint , Track     );
	DECL_GETTER_SETTER         (      uint , GenreIndex);
	DECL_GETTER_SETTER_MODIFIED(const char*, Genre     );
#undef DECL_GETTER_SETTER

	bool serialize(const uchar* f_pData, uint f_size, bool f_bResetModified = true);
		
private:
	CID3v1(const Tag& f_tag);
	CID3v1();

private:
	bool m_v11;

	char m_title[31];
	char m_artist[31];
	char m_album[31];
	char m_year[5];
	char m_comment[31];
	uint m_track;
	uint m_genre;

	uint m_maskModified;
};

#endif // __ID3_V1_H__

