#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>


#define ASSERT(expr) do { if(!(expr)) { TRACE( QString("ASSERT(%1)").arg(#expr) ); abort(); } } while(0)


#define TRACE(msg) trace(msg, __FILE__, QString::number(__LINE__))

static inline void trace(const QString& f_msg, const QString& f_file, const QString& f_line)
{
	QFileInfo f(f_file);
	qDebug() << (QDateTime::currentDateTime().toString(Qt::DefaultLocaleShortDate) + QString(" @ ") +
				 f.fileName() + "(" + f_line + ")\t" +
				 f_msg);
}

// ============================================================================
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
	{
		TRACE( QString("ERROR (%1): %2").arg(m_brief).arg(m_full) );
	}
	const QString& brief() const { return m_brief; }
	const QString& full() const { return m_full; }
private:
	const QString m_brief;
	const QString m_full;
};

#endif // __DEBUG_H__
