#ifndef __JOB_FILE__
#define __JOB_FILE__


class CID3v1;
class CID3v2;


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
	QSharedPointer<CID3v1> m_tag;
	QSharedPointer<CID3v2> m_tag2;

private:
	QString m_file;
};


class CJobSingle : public CJob
{
public:
	static CJobSingle* create(QWidget& f_parent, const QString& f_file);

private:
	CJobSingle(const QString& f_file):
		CJob(f_file)
	{}

	void updateUI() const;
};

#endif // __JOB_FILE__
