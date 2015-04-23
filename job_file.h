#ifndef __JOB_FILE_H__
#define __JOB_FILE_H__

#include "mp3.h"


class CJob
{
public:
	virtual ~CJob() {}

	virtual bool save() const = 0;

protected:
	CJob(QWidget* pParent, const QString& f_path);

protected:
	QString m_path;

	CMP3 m_mp3;
};


class CJobSingle : public CJob
{
public:
	CJobSingle(QWidget* pParent, const QString& f_path): CJob(pParent, f_path) {}

	virtual bool save() const;

	void updateTag1UI  (Ui::Window& f_ui) const;
	void updateTag2UI  (Ui::Window& f_ui) const;
	void updateMPEGInfo(Ui::Window& f_ui) const;

private:
	void trackTag1UI(Ui::Window& f_ui) const;
	void trackTag2UI(Ui::Window& f_ui) const;
};

#endif // __JOB_FILE_H__
