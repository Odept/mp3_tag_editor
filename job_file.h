#ifndef __JOB_FILE__
#define __JOB_FILE__


class CMP3;


class CJob
{
public:
	virtual ~CJob();

protected:
	CJob(const QString& f_file):
		m_file(f_file)
	{}

	virtual bool init(QWidget& f_parent);

protected:
	QSharedPointer<CMP3> m_mp3;

private:
	QString m_file;
};


class CJobSingle : public CJob
{
public:
	static CJobSingle* create(QWidget& f_parent, const QString& f_file);

	void updateTag1UI(Ui::Window& f_ui) const;
	void updateTag2UI(Ui::Window& f_ui) const;

private:
	CJobSingle(const QString& f_file):
		CJob(f_file)
	{}

	void updateControl(TextEdit& f_control, const QString& f_str) const;
	void updateControl(TextEdit& f_control, const std::string& f_str) const
	{
		updateControl(f_control, QString::fromStdString(f_str));
	}
};

#endif // __JOB_FILE__
