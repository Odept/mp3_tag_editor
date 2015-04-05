#ifndef __JOB_FILE_H__
#define __JOB_FILE_H__

#include "mp3.h"


class CJob
{
public:
	virtual ~CJob() {}

protected:
	CJob(const QString& f_path);

protected:
	QString m_path;

	CMP3 m_mp3;
};


class CJobSingle : public CJob
{
public:
	CJobSingle(const QString& f_path): CJob(f_path) {}

	void updateTag1UI(Ui::Window& f_ui) const;
	void updateTag2UI(Ui::Window& f_ui) const;

private:
	void updateControl(TextEdit& f_control, const QString& f_str) const;
	void updateControl(TextEdit& f_control, const std::string& f_str) const
	{
		updateControl(f_control, QString::fromStdString(f_str));
	}
};

#endif // __JOB_FILE_H__
