
#include <stdio.h>
#include <string.h>

#include <libavcodec/avcodec.h>

#define INBUF_SIZE 4096

/**
 * @ingroup lavc_decoding
 * Required number of additionally allocated bytes at the end of the input bitstream for decoding.
 * This is mainly needed because some optimized bitstream readers read
 * 32 or 64 bit at once and could read over the end.<br>
 * Note: If the first 23 bits of the additional bytes are not 0, then damaged
 * MPEG bitstreams could cause overread and segfault.
 */
#define AV_INPUT_BUFFER_PADDING_SIZE 64

void main(){
    printf("wqw\n");

    printf("%d\n",8);
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    inbuf + INBUF_SIZE;
    printf("%d",sizeof(inbuf)/sizeof(inbuf[0]));
    // memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);
}