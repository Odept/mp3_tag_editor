#ifndef __JOB_FILE_H__
#define __JOB_FILE_H__

#include "mp3.h"


class CJob
{
public:
	virtual ~CJob() {}

	virtual bool save(Ui::Window&) = 0;

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

	virtual bool save(Ui::Window& f_ui);
	void sync(Ui::Window& f_ui, uint f_uTag);

	void updateTag1UI  (Ui::Window& f_ui) const;
	void updateTag2UI  (Ui::Window& f_ui) const;
	void updateMPEGInfo(Ui::Window& f_ui) const;

private:
	void syncTag1UI(Ui::Window& f_ui);
	void syncTag2UI(Ui::Window& f_ui);

	template<typename T>
	void syncControl(const T& f_control,
					 const QString& f_text,
					 const QString& f_name,
					 std::function<bool (const QString&)> f_lambda);
};

#endif // __JOB_FILE_H__
