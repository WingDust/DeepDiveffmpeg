


#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
// #include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

// 测试读取的帧数据的正确，所以写入到一个jpg文件来做测试
// [Read/write JPEG image with libjpeg](https://gist.github.com/kentakuramochi/f64e7646f1db8335c80f131be8359044)
#include <jpeglib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct 
{
    uint8_t *data;
    uint32_t width;
    uint32_t height;
    uint32_t ch; // color chanel
} JpegData;


bool wirte_jpg(
    const JpegData *jpegData,
    const char *dstfile,
    struct jpeg_error_mgr *jerr
){
    struct jpeg_compress_struct cinfo;
    jpeg_create_compress(&cinfo);
    cinfo.err = jpeg_std_error(jerr);

    FILE *fp = fopen(dstfile, "wb");
    if (fp == NULL) {
        printf("Error: failed to open %s\n", dstfile);
        return false;
    }

    // 2.
    jpeg_stdio_dest(&cinfo, fp);

    // 3.
    cinfo.image_width      = jpegData->width;
    cinfo.image_height     = jpegData->height;
    cinfo.input_components = jpegData->ch;
    cinfo.in_color_space   = JCS_RGB;
    jpeg_set_defaults(&cinfo);

    // 4.
    jpeg_start_compress(&cinfo, TRUE);

    // 5.
    uint8_t *row = jpegData->data;
    const uint32_t stride = jpegData->width * jpegData->ch;
    for (int y = 0; y < jpegData->height; y++) {
        jpeg_write_scanlines(&cinfo, &row, 1);
        row += stride;
    }

    // 6.
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(fp);

    return true;
}


int main (int argc,char *argv[]){

        // char *f = "./1.flv";
        char *f = "./1.mp4";

    AVFormatContext *fmt_ctx = NULL;

    AVFrame *frame =  NULL;

    // if (argc != 2) {
    //     fprintf(stderr, "Usage: %s <video>\n", argv[0]);
    //     exit(1);
    // }

    // if (avformat_open_input(&fmt_ctx,argv[1],NULL,NULL) < 0/* condition */)
    if (avformat_open_input(&fmt_ctx,f,NULL,NULL) < 0/* condition */)
    {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }
// Retrieve stream information.
	if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
		return -1; // Couldn't find stream information.
	}
    // 解码
    // AVCodec *dec = NULL;

    // 用于函数成功的返回值判断，会被多个函数返回赋值
    int ret;

    // 找到视频流的索引，与视频流的默认使用的解码器
    // int  video_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1,&dec,0);
    int  video_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1,NULL,0);
    printf("%d",video_stream_idx );
    // printf("",fmt_ctx->streams[video_stream_idx ]);
    // 将上面找到的解码器分配 AVCodecContext 与内存

    AVCodecParameters * avcodecParameters = fmt_ctx->streams[video_stream_idx]->codecpar;
    enum AVCodecID codecId = avcodecParameters->codec_id;

    AVCodec * codec = avcodec_find_decoder(codecId);

    AVCodecContext *dec_ctx = avcodec_alloc_context3(NULL);
    // AVCodecContext *dec_ctx = avcodec_alloc_context3(dec);
    if (!dec_ctx) {
        fprintf(stderr, "Failed to allocate codec\n");
        return AVERROR(EINVAL);
    }
    // 再将视频流中的参数加载到 AVCodecContext上
    ret = avcodec_parameters_to_context(dec_ctx,avcodecParameters);

    if (ret < 0) {
        fprintf(stderr, "Failed to copy codec parameters to codec context\n");
        return ret;
    }
    // 打开解码器
    ret = avcodec_open2(dec_ctx,codec,0);
        if (ret < 0) {
            fprintf(stderr, "Failed to open %s codec\n");
            return ret;
        }

    frame = av_frame_alloc();
    AVFrame *framergb = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
    }
    AVPacket *pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "Could not allocate AVPacket\n");
    }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24,dec_ctx->width,dec_ctx->height,1);
	uint8_t *buffer = NULL;
    buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

    av_image_fill_arrays(framergb->data,framergb->linesize,buffer,AV_PIX_FMT_RGB24,dec_ctx->width,dec_ctx->height,1);

	struct SwsContext *sws_ctx = NULL;


    sws_ctx = sws_getContext(
        avcodecParameters->width,
        avcodecParameters->height,
        dec_ctx->pix_fmt,
        avcodecParameters->width,
        avcodecParameters->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,NULL,NULL,NULL);
    // sws_ctx = sws_getContext(dec_ctx->width,dec_ctx->height,dec_ctx->pix_fmt,dec_ctx->width,dec_ctx->height,AV_PIX_FMT_RGB24,SWS_BILINEAR,NULL,NULL,NULL);

    // 读取帧
    int currentIdx=0;

    /**
     * ffprobe -v error -count_frames -select_streams v:0 -show_entries stream=nb_read_frames -of default=nokey=1:noprint_wrappers=1 1.mp4
     * 经过测试 av_read_frame 并 avcodec_receive_frame 成功与上面 ffprobe 从命令行下得到的视频总帧数，有过一帧的差距
     */

    while (av_read_frame(fmt_ctx,pkt)>=0/* condition */)
    {
    if (pkt->stream_index == video_stream_idx){
        avcodec_send_packet(dec_ctx,pkt);

        ret = avcodec_receive_frame(dec_ctx,frame);
        // 解码成功
        if (ret==0)
        {
            // av_write_frame(fmt_ctx,pkt);
            sws_scale(sws_ctx,
            (uint8_t const * const *) frame->data,
            frame->linesize,
            0,
            dec_ctx->height,
            framergb->data,
            framergb->linesize
            );
            currentIdx++;
            char *dst = "./out.rgb";
            char *yuv = "./out.yuv";
            if (currentIdx==1){
                // FILE *o = fopen(dst,"wb+");
                // fwrite(framergb->data[0],(frame->width)*(frame->height)*3,1,o);
                // FILE *o = fopen(yuv,"wb+");
                // fwrite(frame->data[0],1,(frame->width)*(frame->height),o);
                // fwrite(frame->data[1],1,(frame->width)*(frame->height)/4,o);
                // fwrite(frame->data[2],1,(frame->width)*(frame->height)/4,o);
                // fclose(o);
            }
            // if (currentIdx==4894) {
            // printf("%d\n",2);
            // }
            // printf("%d\n",currentIdx);
        }
    }
    }
    // ret = av_read_frame(fmt_ctx,pkt);
    // if (pkt->stream_index == video_stream_idx){
    //     ret = avcodec_send_packet(dec_ctx,pkt);
    //     if (ret < 0) {
    //     fprintf(stderr, "Error sending a packet for decoding\n");
    //     exit(1);
    //     }
    //     else
    //     {
    //         // AVERROR(EAGAIN);
    //         ret = avcodec_receive_frame(dec_ctx,frame);
    //         // 解码成功
    //         if (ret == 0)
    //         {
    //             /* code */
    //         }
            
    //         sws_scale(sws_ctx,(uint8_t const * const *) frame->data,frame->linesize,0,dec_ctx->height,framergb->data,framergb->linesize);

    //         // char *dst = "./out.jpg";
    //         //  struct jpeg_error_mgr jerr;
    //         // JpegData jpg = {
    //         //     .data = framergb->data[0],
    //         //     .width = framergb->width,
    //         //     .height = framergb->height,
    //         //     .ch = 3,
    //         // };

    //         // wirte_jpg(&jpg,dst,&jerr);
    //     }
    // }

    return 0;
}