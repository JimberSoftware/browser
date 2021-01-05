#include "jxfiledownload.h"
#include "JCommandHandler.h"
#include <QHostInfo>
#include <QFile>
#include <QRandomGenerator>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

DownloadManager::DownloadManager() = default;

void DownloadManager::downloadRequested(QWebEngineDownloadItem *download) {

    connect(download, &QWebEngineDownloadItem::downloadProgress, this, &DownloadManager::downloadProgress);
    connect(download, &QWebEngineDownloadItem::finished, this, &DownloadManager::downloadFinished);
    connect(download, &QWebEngineDownloadItem::stateChanged, this, &DownloadManager::downloadStateChanged);

    JxLog() << "A download has been requested" << download->path() << -1;
    QString filename = download->path().split("/").back();

    QString rPath = getRandomPath();
    QString path = "/jimber_downloads/" + rPath + "/" + filename; // PATH TRAVERSAL?
    download->setPath(path);
    m_downloads.push_back(download);

    JCommand(OutgoingCommand::FILEDOWNLOADCREATE, -1) << download->id() << filename << rPath;

    download->accept();
}

void DownloadManager::downloadCancel(int id) {
    m_downloads.at(id - 1)->cancel();
}

void DownloadManager::downloadPause(int id) {
    QWebEngineDownloadItem *download = m_downloads.at(id - 1);
    download->pause();
    if (download->isPaused()) {
        JCommand(OutgoingCommand::FILEDOWNLOADPAUSE, -1) << id << "paused";
    } else {
        qInfo() << "Could not pause the download";
    }
}

void DownloadManager::downloadResume(int id) {
    QWebEngineDownloadItem *download = m_downloads.at(id - 1);
    download->resume();
    if (!download->isPaused()) {
        JCommand(OutgoingCommand::FILEDOWNLOADDOWNLOADING, -1) << id;
    } else {
        qInfo() << "Could not resume the download";
    }
}

void DownloadManager::downloadStateChanged(QWebEngineDownloadItem::DownloadState state) {
    auto *download = reinterpret_cast<QWebEngineDownloadItem *>(sender());
    if (state == QWebEngineDownloadItem::DownloadInProgress) {
        JCommand(OutgoingCommand::FILEDOWNLOADDOWNLOADING, -1) << download->id();
    } else if (state == QWebEngineDownloadItem::DownloadCancelled) {
        JCommand(OutgoingCommand::FILEDOWNLOADCANCEL, -1) << download->id();
    } else if (state == QWebEngineDownloadItem::DownloadCompleted) {
        JCommand(OutgoingCommand::FILEDOWNLOADFINISH, -1) << download->id();
    }
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    JCommand(OutgoingCommand::FILEDOWNLOADPROGRESS, -1)
            << reinterpret_cast<QWebEngineDownloadItem *>(sender())->id()
            << bytesReceived << bytesTotal;
}

void DownloadManager::downloadFinished() {
    auto *download = reinterpret_cast<QWebEngineDownloadItem *>(sender());
    JxLog() << "There is a download m_finished with path: " << download->path() << download->state();
    if (download->mimeType() == "application/pdf") {
        QFile file(download->path());
        if (!file.open(QIODevice::ReadOnly)) {
            return;
        }
        QByteArray blob = file.readAll();
        QString b64blob(blob.toBase64());
        JCommand(OutgoingCommand::PDF, -1) << b64blob;
    }
}

QString DownloadManager::getRandomPath() {
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 24; // assuming you want random strings of 24 characters

    QString randomString;
    for (int i = 0; i < randomStringLength; ++i) {
        int index = QRandomGenerator::global()->generate() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}
