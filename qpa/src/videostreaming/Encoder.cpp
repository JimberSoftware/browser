#include "Encoder.h"

#include "../jxwsserver.h"
#include "../jxlog.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

Encoder::Encoder(QWindow *window, int width, int height, int crf, const char *target, QObject *parent) : QObject(parent),
                                                                                                         _fpsThread(&Encoder::fpsThread, this), _width(width), _height(height), _crf(crf)

{
  _window = window;
  avformat_network_init();

  int err;
  // AVOutputFormat *fmt;
  AVCodec *codec;

  av_register_all();
  fmt_ctx = avformat_alloc_context();
  if (fmt_ctx == nullptr)
  {
    throw AVException(ENOMEM, "can not alloc av context");
  }

  // fmt_ctx->oformat = fmt; seems unneeded since not initialized?
  fmt_ctx->debug = AV_LOG_VERBOSE;
  snprintf(fmt_ctx->filename, sizeof(fmt_ctx->filename), "%s", target);
  //Reference for AvFormatContext options : https://ffmpeg.org/doxygen/2.8/movenc_8c_source.html
  //Set format's privater options, to be passed to avformat_write_header()

  codec = avcodec_find_encoder(OUTPUT_CODEC);
  if (!codec)
  {
    throw AVException(1, "can't find encoder");
  }

  //Set codec_ctx to stream's codec structure
  codec_ctx = avcodec_alloc_context3(codec);
  /* put sample parameters */

  // width = 700;
  // height = 800;
  codec_ctx->time_base.den = 25;
  codec_ctx->time_base.num = 1;
  codec_ctx->width = width;
  codec_ctx->height = height;
  codec_ctx->pix_fmt = OUTPUT_PIX_FMT;
  codec_ctx->gop_size = 5000;
  codec_ctx->max_b_frames = 500;
  /* Apparently it's in the example in master but does not work in V11
  if (o_format_ctx->oformat->flags & AVFMT_GLOBALHEADER)
      codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  */
  //H.264 specific options
  std::string s = std::to_string(crf);
  char const *crfString = s.c_str(); //use char const* as target type

  err = av_opt_set(codec_ctx->priv_data, "crf", crfString, 0);
  if (err < 0)
  {
    std::cerr << "Error crf : " << AVException(err, "av_opt_set crf").what() << std::endl;
  }
  err = av_opt_set(codec_ctx->priv_data, "profile", "baseline", 0);
  if (err < 0)
  {
    std::cerr << "Error : profile " << AVException(err, "av_opt_set profile").what() << std::endl;
  }
  err = av_opt_set(codec_ctx->priv_data, "preset", "ultrafast", 0);
  if (err < 0)
  {
    std::cerr << "Error :  preset" << AVException(err, "av_opt_set preset").what() << std::endl;
  }

  err = av_opt_set(codec_ctx->priv_data, "tune", "zerolatency", 0);
  if (err < 0)
  {
    codec_ctx->time_base.den = 25;
    codec_ctx->time_base.num = 1;
    std::cerr << "Error : " << AVException(err, "av_opt_set preset").what() << std::endl;
  }
  //It's necessary to open stream codec to link it to "codec" (the encoder).
  err = avcodec_open2(codec_ctx, codec, NULL);
  if (err < 0)
  {
    throw AVException(err, "avcodec_open2");
  }

  //* dump av format informations
  std::cout << fmt_ctx << " " << target << " ############# " << std::flush;
  // av_dump_format(fmt_ctx, 0, target, 1);
  //*/
  AVDictionary *options = NULL;
  //av_dict_set(&options, "pkt_size", "130", 0);
  //av_dict_set(&options, "buffer_size", "0", 0);

  av_dict_set(&options, "pkt_size", "5000", 0);
  av_dict_set(&options, "buffer_size", "0", 0);
  int flags = AVIO_FLAG_WRITE;
  flags |= AVIO_FLAG_DIRECT;

  err = avio_open2(&fmt_ctx->pb, target, flags, NULL, &options);
  if (err < 0)
  {
    throw AVException(err, "avio_open");
  }
  frame = av_frame_alloc();

  frame->format = AV_PIX_FMT_YUV420P;
  frame->width = width;
  frame->height = height;
  av_image_alloc(frame->data, frame->linesize, width, height, AV_PIX_FMT_YUV420P, 32);
  if (!frame)
  {
    fprintf(stderr, "Could not allocate video frame\n");
    exit(1);
  }
  _sws_context = sws_getCachedContext(NULL,
                                      width, height, AV_PIX_FMT_RGB24,
                                      width, height, AV_PIX_FMT_YUV420P,
                                      // 394, 656, AV_PIX_FMT_YUV420P,
                                      0, 0, 0, 0);
  _ready = true;
}

//Return 1 if a packet was written. 0 if nothing was done.
// return error_code < 0 if there was an error.

int Encoder::write(AVFrame *frame)
{
  int err;
  int got_output = 1;
  AVPacket pkt = {0};
  
  av_init_packet(&pkt);

  //Set frame pts, monotonically increasing, starting from 0
  // if (frame != NULL)
  frame->pts = pts++; //we use frame == NULL to write delayed packets in destructor
  err = avcodec_encode_video2(this->codec_ctx, &pkt, frame, &got_output);

  if (err < 0)
  {
    std::cout << AVException(err, "encode frame").what() << std::endl;
    return err;
  }
  if (got_output)
  {

    pkt.stream_index = 1;
    /* write the frame */
    //printf("Write packet %03d of size : %05d\n",pkt.pts,pkt.size);
    //write_frame will take care of freeing the packet.
    {
      std::lock_guard<std::mutex> lg(_pktMutex);
      _pkts.push(pkt);
      QByteArray videodata(QByteArray::fromRawData((char *)pkt.data, pkt.size));
      videodata.insert(0, QString("video"));
      emit DataWritten(videodata, _window->winId());
    }

    if (err < 0)
    {
      std::cout << AVException(err, "write frame").what() << std::endl;
      return err;
    }

    return 1;
  }
  else
  {
    return 0;
  }
}

void Encoder::fpsThread()
{
  return;
  while (true)
  {
    if(!_ready){
       std::this_thread::sleep_for(std::chrono::milliseconds(100));
       continue;
    }
    if (_pkts.size() > 0)
    {
      qInfo() << "not reaching??";
      return;
      std::lock_guard<std::mutex> lg(_pktMutex);

      auto pkt = _pkts.front();

      _pkts.pop();
      qInfo() << fmt_ctx->pb << pkt.data << pkt.size;
      avio_write(fmt_ctx->pb, pkt.data, pkt.size);

      //JxWsServer::instance().sendToAllClients(QByteArray::fromRawData((char*)pkt.data, pkt.size));
      av_free_packet(&pkt);

      std::this_thread::sleep_for(std::chrono::milliseconds(38));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    if (_stopped)
    {
      return;
    }
  }
}

AVFrame *Encoder::convertRgbTYuv(uint8_t *rgb)
{
  const int in_linesize[1] = {3 * _width};

  sws_scale(_sws_context, (const uint8_t *const *)&rgb, in_linesize, 0, _height, frame->data, frame->linesize);

  return frame;
}

Encoder::~Encoder()
{

  _stopped = true;
  _fpsThread.join();

  av_free(frame->data[0]);
  av_frame_free(&frame);
  if (fmt_ctx != nullptr)
  {
    avio_close(fmt_ctx->pb);
  }
}