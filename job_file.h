#ifndef __JOB_FILE__
#define __JOB_FILE__


class CID3v1;
class CID3v2;


class CJob
{
public:
	virtual ~CJob();

protected:
	static CJob* init(QWidget& f_parent, CJob* f_pJob)
	{
		if(f_pJob->init(f_parent))
			return f_pJob;
		else
		{
			delete f_pJob;
			return NULL;
		}
	}

	CJob(const QString& f_file):
		m_file(f_file)
	{}

	virtual bool init(QWidget& f_parent);

protected:
	QSharedPointer<CID3v1> m_tag;
	QSharedPointer<CID3v2> m_tag2;

private:
	QString m_file;
};


class CJobSingle : public CJob
{
public:
	static CJobSingle* create(Ui::Window& f_ui, const QString& f_file);

private:
	CJobSingle(Ui::Window& f_ui, const QString& f_file):
		CJob(f_file),
		m_ui(f_ui)
	{}

	void updateUI() const;

private:
	Ui::Window& m_ui;
};

#endif // __JOB_FILE__
