## linux 上使用 vscode debug ffmpeg
  自己已经遵从 [ffmpeg-vscode](https://github.com/neptune46/ffmpeg-vscode) 
  
  生成如如下的目录
  ```txt
1.mp4        compat     CONTRIBUTING.md  COPYING.LGPLv2.1  doc      ffmpeg_g  ffprobe    INSTALL.md   libavfilter    libavutil      libswscale   Makefile   RELEASE  tools
Analysis.md  config.h   COPYING.GPLv2    COPYING.LGPLv3    ffbuild  ffplay    ffprobe_g  libavcodec   libavformat    libpostproc    LICENSE.md   presets    seg.md   tt.c
Changelog    configure  COPYING.GPLv3    CREDITS           ffmpeg   ffplay_g  fftools    libavdevice  libavresample  libswresample  MAINTAINERS  README.md  tests

  ```
  `launch.json` 为
  ```json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "gcc - 生成和调试活动文件",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/ffmpeg_g",
            "args": [
                "-i",
                "1.mp4",
                "-vf",
                "\"select=eq(n\\,0)\"",
                "-vframes",
                "1",
                "1.png"
            ],
            "stopAtEntry": true,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: gcc 生成活动文件",
            "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
}
  ```

 但是运行 Start Debugging button 是
 ```txt
 [Running] cd "/home/wingdust/Programming/FFmpeg/fftools/" && gcc ffmpeg.c -o ffmpeg && "/home/wingdust/Programming/FFmpeg/fftools/"ffmpeg
ffmpeg.c:26:10: 致命错误：config.h：没有那个文件或目录
   26 | #include "config.h"
      |          ^~~~~~~~~~
编译中断。

[Done] exited with code=1 in 0.016 second
 ```
 在根目录,`./configure` 运行后是有 `config.h` 的