#include <QFile>

class JxWebcamMic {

private:
  QFile m_webcam;
  QFile m_mic;

  JxWebcamVideoInfo() : m_webcam("/tmp/campipe"), m_mic("/tmp/micpipe") {

    m_webcam.open(QIODevice::WriteOnly | QIODevice::Append |
                  QIODevice::Unbuffered);

    m_mic.open(QIODevice::WriteOnly | QIODevice::Append |
               QIODevice::Unbuffered);
  }
}