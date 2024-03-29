//
//  FFmpegDecoder.h
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#ifndef MESAI_FFmpegDecoder_H
#define MESAI_FFmpegDecoder_H

#include <iostream>
#include <string>
#include <pthread.h>
#include <functional>
#include <unistd.h>

extern "C" {
#include <libavutil/mathematics.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/pixdesc.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}



namespace MESAI
{
	class FFmpegDecoder
	{
		public:
            FFmpegDecoder(std::string);

			~FFmpegDecoder();

			int intialize();

			void playMedia();

			void finalize();

            void setOnframeCallbackFunction(std::function<void(uint8_t *)> func);

            int width;

            int height;

            int GOP;

            int frameRate;

            int bitrate;

            std::function<void(uint8_t *)> onFrame;

		private:

            std::string path;
			AVDictionary* pAVoptions = NULL;
			AVCodecContext  *pCodecCtx;

			AVFormatContext *pFormatCtx;

            AVFrame *pFrameRGB;
        	AVBitStreamFilterContext* bitstream_filter_ctx_ = nullptr;
            struct SwsContext * img_convert_ctx;

			int videoStream;


	};


}

#endif