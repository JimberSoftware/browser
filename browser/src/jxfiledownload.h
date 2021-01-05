#pragma once
#include <QWebEngineDownloadItem>
#include <QString>
#include "socketcommunication.h"
#include "jxlog.h"

class DownloadManager : public QObject {
public:
  DownloadManager();
  void downloadCancel(int id);
  void downloadPause(int id);
  void downloadResume(int id);

private:
  QList<QWebEngineDownloadItem *> m_downloads;
  static QString getRandomPath();

public slots:
  void downloadRequested(QWebEngineDownloadItem *download);
  void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
  void downloadFinished();
  void downloadStateChanged(QWebEngineDownloadItem::DownloadState state);

};