## ffmpeg 源码分析
  - `fftools/ffmpeg.c` 为 ffmpeg 入口文件
  - `av_read_frame` 和 `av_seek_frame` 方法
  - 解析视频文件流程
  > 先建立 `AVFormatContext` 指针
  > 从 `AVFormatContext` 指针 中找到视频流的编号，因为既有视频流又可能有音频流
  > 再去打开视频的解码器 decoder 初始化解码器，打开解码器
    - avformat_open_input
    > 读取文件，并建立 `AVFormatContext` 指针
    - 
### avformat_open_input
  - 函数实现在 `libavformat/demux.c` 文件中
  
## ffmpeg 注意事项
  - `av_register_all` 已弃用

## ffmpeg 基础知识
  - ·「数据包 Packet」
    > Packet 是流数据中一小块，

## ffmpeg 函数
  - `av_image_fill_arrays`
    - 将 `av_malloc` 所分配的内存瓜分
  
## ffmpeg configure
  - 显示所有可用的解码器
    - `--list-decoders`
  - 显示所有可用的解码器
