#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "debug.h"

#include <QSettings>


class CSettings : public QSettings
{
public:
	CSettings():
		QSettings("Lamtsov Denis", "MP3 Tag Editor"),
		m_kWindowRect("Window/rect")
	{}

	QRect getWindowRect() const
	{
		return contains(m_kWindowRect) ? value(m_kWindowRect).toRect() : QRect();
	}
	void setWindowRect(const QRect& f_r)
	{
		TRACE( QString("Settings: save window rect (%1; %2) %3x%4").
					   arg(f_r.x()).arg(f_r.y()).arg(f_r.width()).arg(f_r.height()) );
		setValue(m_kWindowRect, f_r);
	}

private:
	QString m_kWindowRect;
};

#endif // __SETTINGS_H__
