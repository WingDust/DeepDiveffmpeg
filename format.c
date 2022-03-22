
// #include<stdio.h>
// #include<stdlib.h>
// #include<malloc.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>



typedef struct frameData
{
    uint8_t data[3];
    /* video only */
    /**
     * picture width / height.
     *
     * @note Those fields may not match the values of the last
     * AVFrame output by avcodec_decode_video2 due frame
     * reordering.
     *
     * - encoding: MUST be set by user.
     * - decoding: May be set by the user before opening the decoder if known e.g.
     *             from the container. Some decoders will require the dimensions
     *             to be set by the caller. During decoding, the decoder may
     *             overwrite those values as required while parsing the data.
     */
    int width, height;

} frameData;



int extract(const char *p,frameData *f){
    // 初始化视频格式上下文的空指针
    AVFormatContext *fmt_ctx = NULL;
    // 初始化一个存放视频帧的空指针
    AVFrame *frame =  NULL;

    /**
     * 1.  avformat_open_input 打开视频文件格式上下文并创建流
     * 2.  avformat_find_stream_info 从流中读取到流的信息，如视频帧的格式...，这些信息会挂载在视频格式上下文上
     * 3.  av_find_best_stream 从流中找到视频流在流中的索引,并缓存着
     * 4.  从视频格式上下文中获取解码器配置参数与解码器的ID
     * 5.  使用 avcodec_find_decoder 与解码器的ID 来获取解码器
     * 6.  创建解码器的上下文，为其分配初始化的内存
     * 7.  再将视频流中的参数加载到 AVCodecContext上
     * 8.  打开解码器
     * 9.  分配一个原始读取到帧内存与一个会被转成YUV格式帧的帧内存
     * 10. 分配Packet（Packet 是流数据中一小块）内存
     * 11. 初始化转化格式的上下文结构体
     * 12.  av_image_get_buffer_size 通过指定像素格式、图像宽、图像高来计算所需内存大小
     * 13. av_malloc 分配内存，av_image_fill_arrays 将分配内存填充
     * 14. 
     *      av_read_frame
     *          |
     *          |
     *          v
     *      avcodec_send_packet
     *          |
     *          |
     *          v
     *      avcodec_receive_frame
     *          |
     *          |
     *          v
     *      sws_scale
     *      
     *      av_read_frame 从流中读取一帧，注意这一帧可能是视频帧、音频帧……
     *      avcodec_send_packet 将从流中读取出来的 Packet 格式数据（存放着帧数据）送给解码器解码
     *      avcode_receive_frame 从解码的数据中获取帧，会判断是否获取成功
     *      sws_scale 将帧的像素格式进行转成需要的格式（我转成的是YUV），获取到的帧不是一都是 YUV 格式
     * 
     */

    // 1.
    if (avformat_open_input(&fmt_ctx,p,NULL,NULL) < 0/* condition */){
        // 打开失败返回 1
        return 1;
    }
    // 2.
	if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        // 不能找到流信息返回 2
		return 2;
    }

    // 用于后继函数成功的返回值判断，会被多个函数返回赋值
    int ret;

    // 3.
    int  video_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1,NULL,0);

    // 4.
    AVCodecParameters * avcodecParameters = fmt_ctx->streams[video_stream_idx]->codecpar;
    enum AVCodecID codecId = avcodecParameters->codec_id;

    // 5.
    AVCodec * codec = avcodec_find_decoder(codecId);

    // 6.
    AVCodecContext *dec_ctx = avcodec_alloc_context3(NULL);
    if (!dec_ctx) {
        // 分配解码器内存失败返回 6
        return 6;
    }
    // 7 
    ret = avcodec_parameters_to_context(dec_ctx,avcodecParameters);
    if (ret < 0) {
        // 分配解码器内存失败返回 7
        return 7;
    }
 
    // 8.
    ret = avcodec_open2(dec_ctx,codec,0);
    if (ret < 0) {
        // 打开解码器失败返回 8
        return 8;
    }
    // 9.
    frame = av_frame_alloc();
    if (!frame) {
        // 分配帧内存失败返回
        return 9;
    }
    // 10.
    AVPacket *pkt = av_packet_alloc();
    if (!pkt) {
        return 10;
    }

    // 记录读取的帧数
    int currentIdx=0;

    // 14.
    printf("av_read_frame\n");
    while (av_read_frame(fmt_ctx,pkt)>=0)
    {
        if (pkt->stream_index == video_stream_idx){
        // 
        printf("avcodec_send_packet\n");
        avcodec_send_packet(dec_ctx,pkt);

        // 
        printf("avcodec_receive_frame\n");
        ret = avcodec_receive_frame(dec_ctx,frame);
            if (ret==0){
                currentIdx++;

                if (currentIdx==1)
                {
                    for (size_t i = 0; i < 3; i++)
                    {
                        f->data[i]=*(frame ->data[i]);
                    }
                    // printf("%d",dec_ctx->width);
                    printf("pass height: %d\n",f->height);
                    // printf("pass width: %d\n",f.height);
                    printf("height: %d\n",dec_ctx->height);
                    f->height=dec_ctx->height;
                    f->width=dec_ctx->width;
                    printf("break\n");
                    break;
                }
            }
        }
    }
    
    printf("av_packet_free\n");
    av_packet_free(&pkt);
    printf("avcodec_close\n");
    // av_frame_free(&frame);
    avcodec_close(dec_ctx);
    printf("avformat_free_context\n");
    avformat_free_context(fmt_ctx);

    return 99;
}


// frameData * create_frameData(){
// 	frameData * f = NULL;
// 	f = (frameData*) malloc(sizeof(frameData));
// 	return f;
// }

// void release_frameDate(frameData * f){
// 	if (f!=NULL)
// 	{
// 		free(f);
// 	}
// }


