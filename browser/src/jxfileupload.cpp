#include "jxfileupload.h"
#include <QDebug>
#include <QFile>
#include <QDir>



bool Upload::isFinished()
{
    return m_finished;
}

bool Upload::isCancelled()
{
    return m_cancelled;
}

void Upload::finish()
{
    m_finished = true;
}

void Upload::cancel()
{
    m_cancelled = true;
}


void Upload::addFile(const QString& name, const QByteArray& data)
{
    QString fileName(m_uniquePath);
    fileName.append(name);
    m_files << fileName;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    file.write(data);
    file.close();

    if (m_files.size() >= m_amountOfFiles) {
        finish();
    }
}

QStringList Upload::getFiles()
{
    return m_files;
}

Upload::Upload() : m_finished(false), m_cancelled(false)
{
    m_uniquePath = "/tmp/uploads/" + getRandomPath() + "/";
    QDir().mkdir(m_uniquePath);
}
Upload::~Upload() = default;

QString Upload::getRandomPath()
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 12; // assuming you want random strings of 12 characters

    QString randomString;
    for (int i = 0; i < randomStringLength; ++i)
    {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

QString Upload::getUniquePath()
{
    return m_uniquePath;
}