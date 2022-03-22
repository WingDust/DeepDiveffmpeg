
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
// #include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>



typedef struct frameData
{
    // uint8_t *data[8];
    uint8_t data[8];
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

int extractFrame(char *p,frameData f){
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
    AVFrame *pAVFrameYUV420P = av_frame_alloc();
    if (!frame) {
        // 分配帧内存失败返回
        return 9;
    }
    // 10.
    AVPacket *pkt = av_packet_alloc();
    if (!pkt) {
        return 10;
    }
    // 11.
    // [ffmpeg源码简析（一）libswscale中的sws_getContext，sws_scale()等](https://www.cnblogs.com/linuxAndMcu/p/12119915.html)
   struct SwsContext *sws_ctx = sws_getContext(
         avcodecParameters->width,  // 源图像的宽度
        avcodecParameters->height,  // 源图像的高度
        dec_ctx->pix_fmt,           // 源图像的像素格式
        avcodecParameters->width,   // 目标图像的宽度
        avcodecParameters->height,  // 目标图像的高度
        AV_PIX_FMT_YUV420P,         // 目标图像的像素格式
        SWS_BITEXACT,               // 缩放算法
        NULL,                       // 源图像的滤波器信息, 若不需要传NULL 
        NULL,                       // 目标图像的滤波器信息, 若不需要传NULL
        NULL                        // 特定缩放算法需要的参数，默认为NULL
        );



    // 12.
    // [av_image_get_buffer_size（）函数的作用](https://blog.csdn.net/FY_2018/article/details/81025522)
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,dec_ctx->width,dec_ctx->height,1);

    // 13. 
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes);
    av_image_fill_arrays(
        frame->data,
        frame->linesize,
        buffer,
        AV_PIX_FMT_YUV420P,
        dec_ctx->width,
        dec_ctx->height,
        1);

    // 记录读取的帧数
    int currentIdx=0;

    // 14.
    while (av_read_frame(fmt_ctx,pkt)>=0)
    {
        if (pkt->stream_index == video_stream_idx){
        // 
        avcodec_send_packet(dec_ctx,pkt);

        // 
        ret = avcodec_receive_frame(dec_ctx,frame);
            if (ret==0){
                currentIdx++;

                if (currentIdx==1)
                {
                    // 有一些视频不是YUV格式帧，所以要转成YUV
                    sws_scale(sws_ctx ,(const uint8_t * const *)frame->data,
                            frame->linesize,
                            0,
                            dec_ctx->height,
                            pAVFrameYUV420P->data,
                            pAVFrameYUV420P->linesize);

                    // f.data=pAVFrameYUV420P->data;
                    for (size_t i = 0; i < 8; i++)
                    {
                        f.data[i]=pAVFrameYUV420P->data[i];
                    }
                    f.height=dec_ctx->height;
                    f.width=dec_ctx->width;
                }
            }
        }
    }

}


int main (int argc,char *argv[]){


    char *flv_avc1 = "./formatVideo/1.flv";
    char *mp4_avc1 = "./formatVideo/2.mp4";
    char *mp4_h265 = "./formatVideo/Elecard_about_Tomsk_part3_HEVC_720p.mp4";
    char *ts_h265 = "./formatVideo/bbb_720p_c.ts";
    char *avi_avc1 = "./formatVideo/file_example_AVI_1280_1_5MG.avi";
    char *rmvb_cook = "./formatVideo/rmvb.rmvb";
    char *mkv_avc1 = "./formatVideo/sample_1280x720_surfing_with_audio.mkv";
    char *wem_av1 = "./formatVideo/Stream3_AV1_720p_3.9mbps.webm";
    char *mov_avc1 = "./formatVideo/file_example_MOV_1280_1_4MB.mov";

    // getPixelFormat(flv_avc1);
    // getPixelFormat(mp4_avc1);
    // getPixelFormat(mp4_h265);
    // getPixelFormat(ts_h265);
    // getPixelFormat(avi_avc1);
    // getPixelFormat(rmvb_cook);
    // getPixelFormat(mkv_avc1);
    // getPixelFormat(wem_av1);
    // getPixelFormat(mov_avc1);


    struct frameData frame;
    extract(mp4_avc1,&frame);
    // extractFrame(rmvb_cook,frame);
    printf("%d\n",frame.height);
    printf("%d\n",frame.width);

    for (int i = 0; i < 8; i++)
    {
        printf("%d\n",frame.data[i]);
        printf("%d\n",sizeof(frame.data[i]));
    }
    

    return 0;
}

int extract(char *p,frameData *f){
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
    while (av_read_frame(fmt_ctx,pkt)>=0)
    {
        if (pkt->stream_index == video_stream_idx){
        // 
        avcodec_send_packet(dec_ctx,pkt);

        // 
        ret = avcodec_receive_frame(dec_ctx,frame);
            if (ret==0){
                currentIdx++;

                if (currentIdx==1)
                {
                    int len1 = strlen(frame->data[0]);
                    int len2 = strlen(frame->data[1]);
                    int len3 = strlen(frame->data[2]);
                    printf("%s\n",frame->data[0]);
                    printf("%s\n",frame->data[1]);
                    printf("%s\n",frame->data[2]);
                    printf("%d\n",len1);
                    printf("%d\n",len2);
                    printf("%d\n",len3);
                    // printf("%p\n",frame->data[0]);
                    // printf("%p\n",frame->data[1]);
                    // printf("%p\n",frame->data[2]);
                    int a = sizeof(*frame ->data[0]);
                    // printf("%d\n",a);
                    uint8_t *d[3] = {"11111111","hhhhhhh","3"};
                    uint8_t *d1 = "122223";
                    uint8_t d2 = "255";
                    uint8_t d3 = "256";
                    uint8_t d4 = "25";
                    uint8_t d5 = "26";
                    uint8_t d6 = "2";
                    printf("%s\n",d1);
                    // for (size_t i = 0; i < 8; i++)
                    // {
                    //     f->data[i]=*frame ->data[i];
                    // }
                    // f->height=dec_ctx->height;
                    // f->width=dec_ctx->width;
                }
            }
        }
    }

}



void getPixelFormat(char *p){
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
    AVFrame *pAVFrameYUV420P = av_frame_alloc();
    if (!frame) {
        // 分配帧内存失败返回
        return 9;
    }
    // 10.
    AVPacket *pkt = av_packet_alloc();
    if (!pkt) {
        return 10;
    }
    // 11.
    // [ffmpeg源码简析（一）libswscale中的sws_getContext，sws_scale()等](https://www.cnblogs.com/linuxAndMcu/p/12119915.html)
   struct SwsContext *sws_ctx = sws_getContext(
         avcodecParameters->width,  // 源图像的宽度
        avcodecParameters->height,  // 源图像的高度
        dec_ctx->pix_fmt,           // 源图像的像素格式
        avcodecParameters->width,   // 目标图像的宽度
        avcodecParameters->height,  // 目标图像的高度
        AV_PIX_FMT_YUV420P,         // 目标图像的像素格式
        SWS_BITEXACT,               // 缩放算法
        NULL,                       // 源图像的滤波器信息, 若不需要传NULL 
        NULL,                       // 目标图像的滤波器信息, 若不需要传NULL
        NULL                        // 特定缩放算法需要的参数，默认为NULL
        );



    // 12.
    // [av_image_get_buffer_size（）函数的作用](https://blog.csdn.net/FY_2018/article/details/81025522)
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,dec_ctx->width,dec_ctx->height,1);

    // 13. 
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes);
    av_image_fill_arrays(
        frame->data,
        frame->linesize,
        buffer,
        AV_PIX_FMT_YUV420P,
        dec_ctx->width,
        dec_ctx->height,
        1);

    // 记录读取的帧数
    int currentIdx=0;

    // 14.
    while (av_read_frame(fmt_ctx,pkt)>=0)
    {
        if (pkt->stream_index == video_stream_idx){
        // 
        avcodec_send_packet(dec_ctx,pkt);

        // 
        ret = avcodec_receive_frame(dec_ctx,frame);
            if (ret==0){
                currentIdx++;

                if (currentIdx==1)
                {
                    printf("%d",dec_ctx->pix_fmt);
                    // sws_scale(sws_ctx ,(const uint8_t * const *)frame->data,
                    //         frame->linesize,
                    //         0,
                    //         dec_ctx->height,
                    //         pAVFrameYUV420P->data,
                    //         pAVFrameYUV420P->linesize);

                    // // f.data=pAVFrameYUV420P->data;
                    // for (size_t i = 0; i < 8; i++)
                    // {
                    //     f.data[i]=pAVFrameYUV420P->data[i];
                    // }
                    // f.height=dec_ctx->height;
                    // f.width=dec_ctx->width;
                }
            }
        }
    }

}
