#include "rfx_encode.h"
#include <sys/time.h>
#define BITMAP_CNT 20

int main()
{
	RFX_CONTEXT* context = NULL;
	RFX_MESSAGE* message = NULL;
	RFX_RECT rect;
	BYTE* pSrcData = NULL;
	wStream* s;	
	FILE *fp_bitmap,*fp_encode = NULL;
	int ret;
	int stride;
	struct timeval start, end;
	int interval = 0;
	int i;

	context = rfx_context_new(TRUE);
	if (!context)
	{
	    printf("rfx context new failed\n");
	    goto fail;
	}
	else
	    printf("rfx context new successed\n");

	if (!rfx_context_reset(context, 1920 , 1080))
		goto fail;

	context->mode = RLGR3;
	rfx_context_set_pixel_format(context,PIXEL_FORMAT_BGRA32);
	context->width = 1920;
	context->height = 1080;

	fp_bitmap = fopen("/userdata/bitmap","rb");
	fp_encode = fopen("/tmp/bitmap_encode", "wb");

	rect.x = 0;
	rect.y = 0;
	rect.width = 1920;
	rect.height = 1080;

	stride = 1920 * 4;

        pSrcData = malloc(1920 * 1080 * 4);

	for(i = 0;i < BITMAP_CNT;i++)
	{
		s = Stream_New(NULL, 0xFFFF);
		ret = fread(pSrcData, 1920 * 1080 * 4, 1, fp_bitmap);
		gettimeofday(&start, NULL);
        	if (!(rfx_compose_message(context, s, &rect, 1, pSrcData, rect.width, rect.height, stride)))
        	{
                	printf("compose message failed\n");
                	goto fail;
        	}
                gettimeofday(&end, NULL);
        	interval += (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000;

		int32_t bitmapDataLength = Stream_GetPosition(s);
		printf("encode data len:%d\n",bitmapDataLength);
        	BYTE *bitmapData = Stream_Buffer(s);
        	ret = fwrite ( &bitmapDataLength, sizeof(bitmapDataLength), 1, fp_encode);
        	ret = fwrite ( bitmapData, bitmapDataLength, 1, fp_encode);
		Stream_Free(s,1);
	}

	printf("aver interval:%d\n",interval/20);

	fclose(fp_bitmap);
	fclose(fp_encode);

fail:
	rfx_message_free(context, message);
	rfx_context_free(context);
	return 0;
}
