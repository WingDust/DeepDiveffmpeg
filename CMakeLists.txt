# cmake 最低版本支持
cmake_minimum_required(VERSION 3.13)

# 工程名
project(testffmpeg)

# 定义变量
# set(变量名，变量值)
set(Root ../ )

#向工程中添加特定头文件搜索文件路径
include_directories(../libavcodec ../libavdevice ../libavfilter ../libavformat ../libavresample ../libavutil ../libpostproc ../libswresample ../libswscale ../presets)

#向工程中添加库文件搜索文件路径
link_directories(../libavcodec ../libavdevice ../libavfilter ../libavformat ../libavresample ../libavutil ../libpostproc ../libswresample ../libswscale ../presets)

add_executable(tt tt.c)