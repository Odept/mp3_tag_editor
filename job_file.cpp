#include "window.h"
#include "ui_window.h"

#include "job_file.h"

#include "External/inc/id3v1.h"
#include "External/inc/id3v2.h"

#include <QFileInfo>
#include <QMessageBox>


bool CJob::init(QWidget& f_parent)
{
	QFileInfo fi(m_file);
	if(!fi.exists())
	{
		QString msg = QString("File \"%1\" doesn't exist").arg(m_file);
		QMessageBox::critical(&f_parent, QString("File open error"), msg);
		return false;
	}

	QFile file(m_file);
	if( !file.open(QIODevice::ReadOnly) )
	{
		QString msg = QString("Failed to open \"%1\" (%2)").arg(m_file).arg(file.error());
		QMessageBox::critical(&f_parent, QString("File open error"), msg);
		return false;
	}

	const uchar* pData = file.map(0, file.size());
	file.close();
	if(!pData)
	{
		QString msg = QString("Failed to map \"%1\"").arg(m_file);
		QMessageBox::critical(&f_parent, QString("File open error"), msg);
		return false;
	}

	m_tag = QSharedPointer<CID3v1>(CID3v1::gen(pData, file.size()));
	if(m_tag.isNull())
		m_tag = QSharedPointer<CID3v1>(CID3v1::create());
	m_tag2 = QSharedPointer<CID3v2>(CID3v2::gen(pData, file.size()));
	if(m_tag2.isNull())
		m_tag2 = QSharedPointer<CID3v2>(CID3v2::create());

	return true;
}


CJob::~CJob() {}

// ============================================================================
CJobSingle* CJobSingle::create(QWidget& f_parent, const QString& f_file)
{
	CJobSingle* pJob = new CJobSingle(f_file);

	if( pJob->init(f_parent) )
		return pJob;
	else
	{
		delete pJob;
		return NULL;
	}


}


void CJobSingle::updateUI() const
{
}
