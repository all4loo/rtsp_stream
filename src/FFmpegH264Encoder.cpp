//
//  FFmpegH264Encoder.cpp
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//
#include <iostream>
#include "FFmpegH264Encoder.h"

namespace MESAI
{
	#define INQUEUE_SIZE 30
	#define OUTQUEUE_SIZE 30


	FFmpegH264Encoder::FFmpegH264Encoder()
	{
		pthread_mutex_init(&inqueue_mutex,NULL);
		pthread_mutex_init(&outqueue_mutex,NULL);

	}

	void FFmpegH264Encoder::setCallbackFunctionFrameIsReady(std::function<void()> func)
	{
		onFrame = func;
	}


	void FFmpegH264Encoder::SendNewFrame(uint8_t * RGBFrame) {
    							// std::cout << "[bql] check " << __LINE__ << std::endl ;

		pthread_mutex_lock(&inqueue_mutex);
		if(inqueue.size()<INQUEUE_SIZE)
		{
			inqueue.push(RGBFrame);
		}
		pthread_mutex_unlock(&inqueue_mutex);
	}

	void FFmpegH264Encoder::run()
	{
		    							// std::cout << "[bql] check " << __LINE__ << std::endl ;

		while(true)
		{
			    							// // std::cout << "[bql] check " << __LINE__ << std::endl ;

			if(!inqueue.empty())
			{
				uint8_t * frame;
				pthread_mutex_lock(&inqueue_mutex);
				frame = inqueue.front();
				inqueue.pop();
				pthread_mutex_unlock(&inqueue_mutex);
				if(frame != NULL)
				{
					WriteFrame(frame);
				}
			}
        }
	}

	void FFmpegH264Encoder::SetupCodec(const char *filename, int codec_id)
	{
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		int ret;
		m_sws_flags = SWS_BICUBIC;
		m_frame_count=0;

		avcodec_register_all();
		av_register_all();
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		avformat_alloc_output_context2(&m_oc, NULL, NULL, filename);

		if (!m_oc) {
			avformat_alloc_output_context2(&m_oc, NULL, "avi", filename);
		}
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		if (!m_oc) {
			return;
		}
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		m_fmt = m_oc->oformat;
		m_video_st = NULL;
		m_fmt->video_codec = (AVCodecID)codec_id;
		m_fmt->audio_codec = AV_CODEC_ID_NONE;

		AVStream *st;
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		m_video_codec = avcodec_find_encoder(m_fmt->video_codec);
		if (!(m_video_codec)) {
				return;
		}
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		st = avformat_new_stream(m_oc, m_video_codec);

		if (!st) {
				return;
		}
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		st->id = m_oc->nb_streams-1;

		m_c = st->codec;

		m_c->codec_id = m_fmt->video_codec;
		m_c->bit_rate = m_AVIMOV_BPS;			//Bits Per Second
		m_c->width    = m_AVIMOV_WIDTH;			//Note Resolution must be a multiple of 2!!
		m_c->height   = m_AVIMOV_HEIGHT;		//Note Resolution must be a multiple of 2!!
		m_c->time_base.den = m_AVIMOV_FPS;		//Frames per second
		m_c->time_base.num = 1;
		m_c->gop_size      = m_AVIMOV_GOB;		// Intra frames per x P frames
		m_c->pix_fmt       = AV_PIX_FMT_YUV420P;//Do not change this, H264 needs YUV format not RGB

				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		if (m_oc->oformat->flags & AVFMT_GLOBALHEADER)
			m_c->flags |= CODEC_FLAG_GLOBAL_HEADER;

		m_video_st=st;

				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		AVCodecContext *c = m_video_st->codec;
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		ret = avcodec_open2(c, m_video_codec, NULL);
		if (ret < 0) {
					    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;
		return;
		}
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		//ret = avpicture_alloc(&m_dst_picture, c->pix_fmt, c->width, c->height);
		m_dst_picture = av_frame_alloc();
		m_dst_picture->format = c->pix_fmt;
		m_dst_picture->data[0] = NULL;
        m_dst_picture->linesize[0] = -1;
		m_dst_picture->pts = 0;
        m_dst_picture->width = m_c->width;
        m_dst_picture->height = m_c->height;
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		ret = av_image_alloc(m_dst_picture->data, m_dst_picture->linesize, c->width, c->height, (AVPixelFormat)m_dst_picture->format, 32);
		if (ret < 0) {
			return;
		}
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		//ret = avpicture_alloc(&m_src_picture, AV_PIX_FMT_BGR24, c->width, c->height);
		m_src_picture = av_frame_alloc();
		m_src_picture->format = c->pix_fmt;
		ret = av_image_alloc(m_src_picture->data, m_src_picture->linesize, c->width, c->height, AV_PIX_FMT_BGR24, 24);
		// std::cout << "[bql] c->width " <<c->width << std::endl;
		// std::cout << "[bql] c->height " <<c->height << std::endl;

		if (ret < 0) {
			return;
		}

		bufferSize = ret;

		av_dump_format(m_oc, 0, filename, 1);

		if (!(m_fmt->flags & AVFMT_NOFILE)) {
			ret = avio_open(&m_oc->pb, filename, AVIO_FLAG_WRITE);
			if (ret < 0) {
				return;
			}
		}

		ret = avformat_write_header(m_oc, NULL);

		if (ret < 0) {
			return;
		}

		sws_ctx = sws_getContext(c->width, c->height, AV_PIX_FMT_BGR24,
								 c->width, c->height, AV_PIX_FMT_YUV420P,
								 SWS_BICUBIC, NULL, NULL, NULL);
		if (!sws_ctx) {
			return;
		}
	}

	void FFmpegH264Encoder::WriteFrame(uint8_t * RGBFrame )
	{
		// std::cout << "[bql] bufferSize " <<bufferSize << " m_src_picture->data " << m_src_picture->data <<std::endl;
// return;
		memcpy(m_src_picture->data[0], RGBFrame, bufferSize); //704*576*3
// return;
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		sws_scale(sws_ctx,
					m_src_picture->data, m_src_picture->linesize,
					0, m_c->height, m_dst_picture->data, m_dst_picture->linesize);
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

// return;
        AVPacket pkt = { 0 };
		int got_packet;
		av_init_packet(&pkt);
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		int ret = 0;
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		ret = avcodec_encode_video2(m_c, &pkt, m_dst_picture, &got_packet);
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		if (ret < 0) {
			return;
		}
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		if (!ret && got_packet && pkt.size)
		{
			pkt.stream_index = m_video_st->index;
			FrameStructure * frame = new FrameStructure();
			frame->dataPointer = new uint8_t[pkt.size];
			frame->dataSize = pkt.size-4;
			frame->frameID = m_frame_count;

			memcpy(frame->dataPointer,pkt.data+4,pkt.size-4);

			pthread_mutex_lock(&outqueue_mutex);

			if(outqueue.size()<OUTQUEUE_SIZE)
			{
				outqueue.push(frame);
			}
			else
			{
				delete frame;
			}

			pthread_mutex_unlock(&outqueue_mutex);

		}
				    							// std::cout << "[bql] check " << __FUNCTION__ << ":"<<__LINE__ << std::endl ;

		av_free_packet(&pkt);

		m_frame_count++;
		m_dst_picture->pts += av_rescale_q(1, m_video_st->codec->time_base, m_video_st->time_base);

		onFrame();
	}

	void FFmpegH264Encoder::SetupVideo(std::string filename, int Width, int Height, int FPS, int GOB, int BitPerSecond)
	{
		// std::cout << "[bql] SetupVideo" << std::endl;
		// std::cout << "[bql] SetupVideo" << std::endl;
		// std::cout << "[bql] SetupVideo" << std::endl;
		// std::cout << "[bql] SetupVideo" << std::endl;
		// std::cout << "[bql] SetupVideo" << std::endl;
		m_filename = filename;
		m_AVIMOV_WIDTH=Width;	//Movie width
		m_AVIMOV_HEIGHT=Height;	//Movie height
		m_AVIMOV_FPS=FPS;		//Movie frames per second
		m_AVIMOV_GOB=GOB;		//I frames per no of P frames, see note below!
		m_AVIMOV_BPS=BitPerSecond; //Bits per second, if this is too low then movie will become garbled

		SetupCodec(m_filename.c_str(),AV_CODEC_ID_H264);
	}

	void FFmpegH264Encoder::CloseCodec()
	{

		av_write_trailer(m_oc);
	    avcodec_close(m_video_st->codec);

	    av_freep(&(m_dst_picture->data[0]));
        av_frame_unref(m_dst_picture);
        av_free(m_dst_picture);
        av_freep(&(m_src_picture->data[0]));
        av_frame_unref(m_src_picture);
        av_free(m_src_picture);

	    if (!(m_fmt->flags & AVFMT_NOFILE))
	        avio_close(m_oc->pb);

        m_oc->pb = NULL;

	    avformat_free_context(m_oc);
	    sws_freeContext(sws_ctx);

	}

	void FFmpegH264Encoder::CloseVideo()
	{
		CloseCodec();
	}

	char FFmpegH264Encoder::GetFrame(u_int8_t** FrameBuffer, unsigned int *FrameSize)
	{
		if(!outqueue.empty())
		{
			FrameStructure * frame;
			frame  = outqueue.front();
			*FrameBuffer = (uint8_t*)frame->dataPointer;
			*FrameSize = frame->dataSize;
			return 1;
		}
		else
		{
			*FrameBuffer = 0;
			*FrameSize = 0;
			return 0;
		}
	}

	char FFmpegH264Encoder::ReleaseFrame()
	{
		pthread_mutex_lock(&outqueue_mutex);
		if(!outqueue.empty())
		{
			FrameStructure * frame = outqueue.front();
			outqueue.pop();
			delete frame;
		}
		pthread_mutex_unlock(&outqueue_mutex);
		return 1;
	}
}