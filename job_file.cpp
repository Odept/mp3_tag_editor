#include "window.h"
#include "ui_window.h"

#include "job_file.h"

#include <QFileInfo>
#include <QMessageBox>


bool CJob::init()
{
	QFileInfo fi(m_file);
	if(!fi.exists())
	{
		QString msg = QString("File \"%1\" doesn't exist").arg(m_file);
		QMessageBox::critical(&m_parent, QString("File open error"), msg);
		return false;
	}

	QFile file(m_file);
	if( !file.open(QIODevice::ReadOnly) )
	{
		QString msg = QString("Failed to open \"%1\" (%2)").arg(m_file).arg(file.error());
		QMessageBox::critical(&m_parent, QString("File open error"), msg);
		return false;
	}

	const uchar* pData = file.map(0, file.size());
	file.close();
	if(!pData)
	{
		QString msg = QString("Failed to map \"%1\"").arg(m_file);
		QMessageBox::critical(&m_parent, QString("File open error"), msg);
		return false;
	}

	return true;
}

/*
CJob::~CJob()
{

}
*/
