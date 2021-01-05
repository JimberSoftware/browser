#include <QStringList>
#include <QString>
#include <QSharedPointer>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


class Upload
{
private:
  bool m_finished;
  bool m_cancelled;
  QStringList m_files;
  QString m_uniquePath;
  QString getRandomPath();
  int m_amountOfFiles = 0;

public:
  Upload();
  bool isFinished();
  bool isCancelled();
  QStringList getFiles();
//  int getId();W
  void addFile(const QString& f, const QByteArray &);
  QString getUniquePath();
  void finish();
  void cancel();
  void setAmountOfFiles(int amount){ m_amountOfFiles = amount;};
  ~Upload();
};


QT_END_NAMESPACE