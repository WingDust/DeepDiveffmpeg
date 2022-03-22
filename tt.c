#include <stdio.h>
// #include "../libavutil/avutil.h"
#include "avdevice.h"
#include "avformat.h"
#include "avcodec.h"


void main (){
    // 注册
    avdevice_register_all();

    AVInputFormat *iformat = av_find_input_format("avfoundation");

    AVFormatContext *fmt_ctx = NULL;
    char *deviceName = ":0";
    AVDictionary *options = NULL;
    char errors[1024];
    int ret = avformat_open_input(&fmt_ctx,deviceName,iformat,&options);

    if (ret<0 /* condition */)
    {
        // av_strerror(ret,errors,1024);
        //printf(stderr,"Faild to open audio device, [%d]%s/n", ret, errors);
        printf("error");
        return;
        /* code */
    }
    printf("Hello C.\n");

    int count = 0;
    AVPacket pkt;
    av_init_packet(&pkt);

    //  av_seek_frame()
    while ((ret = av_read_frame(fmt_ctx,&pkt)>=0)/* condition */)
    {
        printf("pkt.size=%d/n", pkt.size);
        /* code */
    }

    av_packet_unref(&pkt);

    if (ret<0/* condition */)
    {
        // printf("%s\n", av_err2str(ret));
        /* code */
    }
    //av_log_set_level(AV_LOG_DEBUG);
    //av_log(NULL, AV_LOG_DEBUG, "Hello, av log!\n");
} 