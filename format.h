
#include <stdint.h>
// [stdint.h中的可移植类型](https://blog.gotocoding.com/archives/652)

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



int extract(const char *p,frameData *f);