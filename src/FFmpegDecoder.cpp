//
//  FFmpegDecoder.cpp
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#include "FFmpegDecoder.h"

//FFMPEG use AVCodecParameters instead of AVCodecContext since from version 3.1(libavformat/version:57.40.100)
#define FFMPEG_VERSION_3_1 AV_VERSION_INT(57,40,100)

namespace MESAI
{
    FFmpegDecoder::FFmpegDecoder(std::string path)
	{
        this->path = path;
	}


	int FFmpegDecoder::intialize()
	{

		// Intialize FFmpeg enviroment
	    av_register_all();
	    avcodec_register_all();
	    avformat_network_init();

	    const char  *filenameSrc = path.c_str();

	    pFormatCtx = avformat_alloc_context();

		av_dict_set(&pAVoptions, "buffer_size", "1024000", 0);
		av_dict_set(&pAVoptions, "stimeout", "200000", 0);

	    AVCodec * pCodec;

	    if(avformat_open_input(&pFormatCtx,filenameSrc,NULL,&pAVoptions) != 0)
	    {
			//exception
			std::cout<<"Couldn't open input stream."<<std::endl;
			return -1;
	    }

	    if(avformat_find_stream_info(pFormatCtx, NULL) < 0)
	    {
	    	//exception
			std::cout<<"Couldn't find stream information."<<std::endl;
	    	return -1;
	    }

	    av_dump_format(pFormatCtx, 0, filenameSrc, 0);

	    videoStream = -1;
	    AVStream* vstream = nullptr;
	    for(int i=0; i < pFormatCtx->nb_streams; i++)
	    {
	        vstream = pFormatCtx->streams[i];
#if LIBAVFORMAT_VERSION_INT>=FFMPEG_VERSION_3_1
			enum AVMediaType type = vstream->codecpar->codec_type;
#else
			enum AVMediaType type = vstream->codec->codec_type;
#endif

			if (type == AVMEDIA_TYPE_VIDEO) {
				videoStream = i;
				break;
			}
	    }

		// std::cout << "[bql] check "  << __LINE__ << std::endl;

	   	// videoStream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO,videoStream, -1, NULL, 0);
		// std::cout << "[bql] check "  << __LINE__ << std::endl;

	    if(videoStream == -1)
	    {
	     	//exception
			std::cout<< "Didn't find a video stream."<<std::endl;
	    	return -1;
	    }
#if LIBAVFORMAT_VERSION_INT>=FFMPEG_VERSION_3_1
  		AVCodecID codec_id = vstream->codecpar->codec_id;
#else
		AVCodecID codec_id = vstream->codec->codec_id;
#endif
  		// bitstream filter
  		bitstream_filter_ctx_ = nullptr;
 		if (strstr(pFormatCtx->iformat->name, "mp4") || strstr(pFormatCtx->iformat->name, "flv") ||
      		strstr(pFormatCtx->iformat->name, "matroska") || strstr(pFormatCtx->iformat->name, "rtsp")) {
    		if (AV_CODEC_ID_H264 == codec_id) {
     			bitstream_filter_ctx_ = av_bitstream_filter_init("h264_mp4toannexb");
			} else if (AV_CODEC_ID_HEVC == codec_id) {
				bitstream_filter_ctx_ = av_bitstream_filter_init("hevc_mp4toannexb");
			}
		}
		// std::cout << "[bql] check "  << __LINE__ << std::endl;

	    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
		// std::cout << "[bql] check "  << __LINE__ << std::endl;

	    pCodec =avcodec_find_decoder(pCodecCtx->codec_id);
	    if(pCodec==NULL)
	    {
	    	//exception
	    	return -1;
	    }
		// std::cout << "[bql] check "  << __LINE__ << std::endl;

	    pCodecCtx->codec_id = pCodec->id;
	    pCodecCtx->workaround_bugs   = 1;
		// std::cout << "[bql] check "  << __LINE__ << std::endl;

	    if(avcodec_open2(pCodecCtx,pCodec,NULL) < 0)
	    {
	    	 //exception
	    	return -1;
	    }
		// std::cout << "[bql] check "  << __LINE__ << std::endl;

        pFrameRGB = av_frame_alloc();
        AVPixelFormat  pFormat = AV_PIX_FMT_BGR24;
        uint8_t *fbuffer;
        int numBytes;
        numBytes = avpicture_get_size(pFormat,pCodecCtx->width,pCodecCtx->height) ; //AV_PIX_FMT_RGB24
        fbuffer = (uint8_t *) av_malloc(numBytes*sizeof(uint8_t));
        avpicture_fill((AVPicture *) pFrameRGB,fbuffer,pFormat,pCodecCtx->width,pCodecCtx->height);
		// std::cout << "[bql] check "  << __LINE__ << std::endl;

        img_convert_ctx = sws_getCachedContext(NULL,pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,   pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL,NULL);
		// std::cout << "[bql] check "  << __LINE__ << std::endl;

        height = pCodecCtx->height;
        width =  pCodecCtx->width;
        bitrate =pCodecCtx->bit_rate;
		if(pCodecCtx->gop_size>0){
			GOP = pCodecCtx->gop_size;
		}
		else
		{
			GOP = 100;
		}
		
        	
		if(pFormatCtx->streams[videoStream]->avg_frame_rate.den>0&&pFormatCtx->streams[videoStream]->avg_frame_rate.num>0){
			frameRate = (int )pFormatCtx->streams[videoStream]->avg_frame_rate.num/pFormatCtx->streams[videoStream]->avg_frame_rate.den;
		}        	
		else
		{
			frameRate = 25;
		}
		
		return 0;
    }

    void FFmpegDecoder::setOnframeCallbackFunction(std::function<void(uint8_t *)> func)
    {
        onFrame = func;
    }


	void FFmpegDecoder::playMedia()
	{
		// std::cout << "[bql] check playMedia" << std::endl ;

		AVPacket packet;
        AVFrame * pFrame;
		while((av_read_frame(pFormatCtx,&packet)>=0))
    	{
			// std::cout << "[bql] check " << __LINE__ << std::endl ;
        	if(packet.buf != NULL & packet.stream_index == videoStream)
        	{
				// std::cout << "[bql] check " << __LINE__ << std::endl ;
                pFrame = av_frame_alloc();
				// std::cout << "[bql] check " << __LINE__ << std::endl ;

                int frameFinished;
                int decode_ret = avcodec_decode_video2(pCodecCtx,pFrame,&frameFinished,&packet);
				// std::cout << "[bql] check " << __LINE__ << std::endl ;

                av_free_packet(&packet);
				// std::cout << "[bql] check " << __LINE__ << std::endl ;

                if(frameFinished)
                {
						// std::cout << "[bql] check " << __LINE__ << std::endl ;

                    sws_scale(img_convert_ctx, ((AVPicture*)pFrame)->data, ((AVPicture*)pFrame)->linesize, 0, pCodecCtx->height, ((AVPicture *)pFrameRGB)->data, ((AVPicture *)pFrameRGB)->linesize);
					// std::cout << "[bql] " << pFrameRGB<< " " << __LINE__ << std::endl;
					// std::cout << "[bql] " << ((AVPicture *)pFrameRGB)->data<< " " << __LINE__ << std::endl;
					// // std::cout << "[bql] " << (((AVPicture *)pFrameRGB)->data[0])<< " " << __LINE__ << std::endl;

                    onFrame(((AVPicture *)pFrameRGB)->data[0]);
					// std::cout << std::endl;
					// std::cout << std::endl;
					// std::cout << std::endl;
					// std::cout << std::endl;
					// std::cout << "[bql] " << ((AVPicture*)pFrame) << " " << __LINE__ << std::endl;
                }
				// std::cout << "[bql] check " << __LINE__ << std::endl ;

                av_frame_unref(pFrame);
				// std::cout << "[bql] check " << __LINE__ << std::endl ;

                av_free(pFrame);
				// std::cout << "[bql] check " << __LINE__ << std::endl ;

        	}
			// std::cout << "[bql] check " << __LINE__ << std::endl ;
			// std::cout << "[bql] check " << __LINE__ << std::endl ;
			// std::cout << "[bql] check " << __LINE__ << std::endl ;

            usleep(((double)(1.0/frameRate))*1000000);
            // usleep(40000);
			// std::cout << "[bql] check " << __LINE__ << std::endl ;
			// std::cout << "[bql] check " << __LINE__ << std::endl ;
			// std::cout << "[bql] check " << __LINE__ << std::endl ;
			// std::cout << "[bql] check " << __LINE__ << std::endl ;


    	}
    	av_free_packet(&packet);


	}

	void FFmpegDecoder::finalize()
	{
        sws_freeContext(img_convert_ctx);
        av_freep(&(pFrameRGB->data[0]));
        av_frame_unref(pFrameRGB);
        av_free(pFrameRGB);
		if (bitstream_filter_ctx_) {
			av_bitstream_filter_close(bitstream_filter_ctx_);
			bitstream_filter_ctx_ = nullptr;
		}
		if(pAVoptions){
			av_dict_free(&pAVoptions);
		}
        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);
    }

}
