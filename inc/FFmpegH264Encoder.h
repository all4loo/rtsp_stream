//
//  FFmpegH264Encoder.h
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#ifndef MESAI_FFMPEGH264_ENCODER_H
#define MESAI_FFMPEGH264_ENCODER_H

#include <pthread.h>
#include <functional>
#include <queue>
#include <string>

extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

// bql
#define AV_CODEC_FLAG_GLOBAL_HEADER (1 << 22)
#define CODEC_FLAG_GLOBAL_HEADER AV_CODEC_FLAG_GLOBAL_HEADER
#define AVFMT_RAWPICTURE 0x0020

namespace MESAI {
class FrameStructure {
 public:
  uint8_t *dataPointer;
  int dataSize;
  int frameID;
  ~FrameStructure() { delete dataPointer; }
};

class FFmpegH264Encoder {
 public:
  FFmpegH264Encoder();
  ~FFmpegH264Encoder();

  void setCallbackFunctionFrameIsReady(std::function<void()> func);

  void SetupVideo(std::string filename, int Width, int Height, int FPS, int GOB,
                  int BitPerSecond);
  void CloseVideo();
  void SetupCodec(const char *filename, int codec_id);
  void CloseCodec();

  void SendNewFrame(uint8_t *RGBFrame);
  void WriteFrame(uint8_t *RGBFrame);
  char ReleaseFrame();

  void run();
  char GetFrame(u_int8_t **FrameBuffer, unsigned int *FrameSize);

 private:
  std::queue<uint8_t *> inqueue;
  pthread_mutex_t inqueue_mutex;
  std::queue<FrameStructure *> outqueue;
  pthread_mutex_t outqueue_mutex;

  int m_sws_flags;
  int m_AVIMOV_FPS;
  int m_AVIMOV_GOB;
  int m_AVIMOV_BPS;
  int m_frame_count;
  int m_AVIMOV_WIDTH;
  int m_AVIMOV_HEIGHT;
  std::string m_filename;

  double m_video_time;

  AVCodecContext *m_c;
  AVStream *m_video_st;
  AVOutputFormat *m_fmt;
  AVFormatContext *m_oc;
  AVCodec *m_video_codec;
  AVFrame *m_src_picture, *m_dst_picture;
  SwsContext *sws_ctx;
  int bufferSize;

  std::function<void()> onFrame;
};
}  // namespace MESAI
#endif