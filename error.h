#ifndef __ERROR_H__
#define __ERROR_H__

#pragma once

#include <QString>


#define NEW_EXCEPT(expr, pParent) \
	do { \
		try \
		{ \
			expr; \
		} \
		catch(const Error& e) \
		{ \
			QMessageBox::critical((pParent), e.brief(), e.full()); \
		} \
	} while(0)


class Error
{
public:
	Error(const QString& f_brief, const QString& f_full):
		m_brief(f_brief),
		m_full(f_full)
	{}
	const QString& brief() const { return m_brief; }
	const QString& full() const { return m_full; }
private:
	const QString m_brief;
	const QString m_full;
};

#endif // __ERROR_H__
