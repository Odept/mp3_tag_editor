#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "debug.h"

#include <QSettings>


class CSettings : public QSettings
{
public:
	CSettings():
		QSettings("Lamtsov Denis", "MP3 Tag Editor"),
		m_kWindowRect("Window/rect"),
		m_kOpenDir("Dialog/OpenDir"),
		m_logFmtNoKey("Settings: no \"%1\" key, return default value")
	{}

	QRect getWindowRect() const
	{
		if( contains(m_kWindowRect) )
			return value(m_kWindowRect).toRect();
		else
		{
			TRACE(m_logFmtNoKey.arg(m_kWindowRect));
			return QRect();
		}
	}
	void setWindowRect(const QRect& f_r)
	{
		TRACE( QString("Settings: save window rect (%1; %2) %3x%4").
					   arg(f_r.x()).arg(f_r.y()).arg(f_r.width()).arg(f_r.height()) );
		setValue(m_kWindowRect, f_r);
	}

	QString getOpenDir() const
	{
		if( contains(m_kOpenDir) )
			return value(m_kOpenDir).toString();
		else
		{
			TRACE(m_logFmtNoKey.arg(m_kOpenDir));
			return QString();
		}
	}
	void setOpenDir(const QString& f_dir)
	{
		TRACE( QString("Settings: save open dir \"%1\"").arg(f_dir) );
		setValue(m_kOpenDir, f_dir);
	}

private:
	QString m_kWindowRect;
	QString m_kOpenDir;

	QString m_logFmtNoKey;
};

#endif // __SETTINGS_H__
