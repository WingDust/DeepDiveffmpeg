


#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
// #include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

// 测试读取的帧数据的正确，所以写入到一个jpg文件来做测试
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
    AVFormatContext *fmt_ctx = NULL;

    AVFrame *frame =  NULL;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <video>\n", argv[0]);
        exit(1);
    }

    if (avformat_open_input(&fmt_ctx,argv[1],NULL,NULL) < 0/* condition */)
    {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }
    // 解码
    AVCodec *dec = NULL;

    // 用于函数成功的返回值判断，会被多个函数返回赋值
    int ret;

    // 找到视频流的索引，与视频流的默认使用的解码器
    int  video_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1,&dec,0);
    printf("%d",video_stream_idx );
    // printf("",fmt_ctx->streams[video_stream_idx ]);
    // 将上面找到的解码器分配 AVCodecContext 与内存
    AVCodecContext *dec_ctx = avcodec_alloc_context3(NULL);
    // AVCodecContext *dec_ctx = avcodec_alloc_context3(dec);
    if (!dec_ctx) {
        fprintf(stderr, "Failed to allocate codec\n");
        return AVERROR(EINVAL);
    }
    // 再将视频流中的参数加载到 AVCodecContext上
    AVCodecParameters * avcodecParameters = fmt_ctx->streams[video_stream_idx]->codecpar;
    ret = avcodec_parameters_to_context(dec_ctx,avcodecParameters);

    if (ret < 0) {
        fprintf(stderr, "Failed to copy codec parameters to codec context\n");
        return ret;
    }
    // 打开解码器
    ret = avcodec_open2(dec_ctx,dec,0);
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

    ret = av_read_frame(fmt_ctx,pkt);
    if (pkt->stream_index == video_stream_idx){
        ret = avcodec_send_packet(dec_ctx,pkt);
        if (ret < 0) {
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
        }
        else
        {
            ret = avcodec_receive_frame(dec_ctx,frame);
            sws_scale(sws_ctx,(uint8_t const * const *) frame->data,frame->linesize,0,dec_ctx->height,framergb->data,framergb->linesize);

            char *dst = "./out.jpg";
             struct jpeg_error_mgr jerr;
            JpegData jpg = {
                .data = framergb->data[0],
                .width = framergb->width,
                .height = framergb->height,
                .ch = 3,
            };

            wirte_jpg(&jpg,dst,&jerr);
        }
    }

    return 0;
}