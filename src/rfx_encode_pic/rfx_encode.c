#include "rfx_encode.h"
#include <sys/time.h>

int main()
{
	RFX_CONTEXT* context = NULL;
	RFX_MESSAGE* message = NULL;
	RFX_RECT rect;
	BYTE* pSrcData = NULL;
	wStream* s;	
	FILE *f,*fp = NULL;
	BITMAPFILEHEADER bitmap_file_header;
	BITMAPINFOHEADER bitmap_info_header;
	RGBQUAD bitmap_rgb;
	int ret;
	int stride;
	struct timeval start, end;
	int interval;

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

	s = Stream_New(NULL, 0xFFFF);

	pSrcData = malloc(1920 * 1080 * 4);

	fp = fopen("/tmp/0.bmp","rb");
	ret = fread ( &bitmap_file_header, 1, sizeof(BITMAPFILEHEADER), fp);
	ret = fread ( &bitmap_info_header, 1, sizeof(BITMAPINFOHEADER), fp);
	ret = fread ( &bitmap_rgb, 1, sizeof(RGBQUAD), fp);
	fclose(fp);


	rect.x = 0;
	rect.y = 0;
	rect.width = 1920;
	rect.height = 1080;

	stride = 1920 * 4;

	gettimeofday(&start, NULL);
	if (!(rfx_compose_message(context, s, &rect, 1, pSrcData, rect.width, rect.height, stride)))
	{
		printf("compose message failed\n");
		goto fail;
	}
	else
	{
		printf("compose message successed\n");
	}
	gettimeofday(&end, NULL);
	interval = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000;
	printf("interval:%d\n",interval);

	int bitmapDataLength = Stream_GetPosition(s);
	BYTE *bitmapData = Stream_Buffer(s);
	f = fopen("/tmp/encode.data", "w");
        ret = fwrite ( bitmapData, bitmapDataLength, 1, f);
        fclose(f);

fail:
	rfx_message_free(context, message);
	rfx_context_free(context);
	return 0;
}
