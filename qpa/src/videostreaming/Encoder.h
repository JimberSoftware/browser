#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QWindow>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h> //for av_image_alloc only
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

QT_BEGIN_NAMESPACE

class AVException : public std::exception
{

  std::string _message;

public:
  AVException(int err, std::string message)
  {
    this->_message = std::to_string(err) + " " + message;
  };
  ~AVException() throw(){};
  std::string getMessage(void)
  {
    return _message;
  };

  virtual const char *what() const throw()
  {
    return _message.c_str();
  }
};

#define OUTPUT_CODEC AV_CODEC_ID_H264
//Input pix fmt is set to BGR24
#define OUTPUT_PIX_FMT AV_PIX_FMT_YUV420P
class Encoder : public QObject
{
  Q_OBJECT
private:
  std::thread _fpsThread;
  int _width;
  int _height;
  int _crf;
  AVFormatContext *fmt_ctx;
  AVCodecContext *codec_ctx; //a shortcut to st->codec
  AVStream *st;
  AVFrame *frame;

  std::queue<AVPacket> _pkts;
  std::mutex _pktMutex;
  AVCodec *codec;
  // SwsContext *_swsContext;
  int pts = 0;
  SwsContext *_sws_context;

  void fpsThread();
  bool _stopped = false;

  bool _ready = false;
  QWindow *_window;

public:
  Encoder(QWindow *window, int width, int height, int crf, const char *target, QObject *parent = nullptr);

  AVFrame *convertRgbTYuv(uint8_t *rgb);
  virtual ~Encoder();
  int write(AVFrame *frame);
signals:
  void DataWritten(QByteArray data, int windowId);
};

QT_END_NAMESPACE

#endif