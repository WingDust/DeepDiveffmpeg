import av
import numpy as np


container = av.open('./formatVideo/2.mp4')

for frame in container.decode(video=0):
# for packet in container.demux():
# for frame in packet .decode():
# for frame in packet .decode():
    # print(frame.to_image())
    frame.to_image().save('{:04d}.jpg'.format(frame.pts),quality=80)
    # print(type(frame.to_image()))
    # img = frame.to_image()
    # arr = np.asarray(img)
    # print(arr)
        # if frame.type == 'video':
            # print(img)


    # print(frame)