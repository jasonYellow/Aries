#include "rfx_encode.h"

int main()
{
	RFX_CONTEXT* context = NULL;
	RFX_MESSAGE* message = NULL;
	RFX_RECT rect;
	BYTE* pSrcData = NULL;
	wStream* s;	
	FILE *f,*fp = NULL;
	BITMAPFILEHEADER *bitmap_file_header;
	BITMAPV5HEADER *bitmapv5_header;
	int ret;
	int stride;

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
	stride = 1920 * 4;

        bitmap_file_header = malloc(sizeof(BITMAPFILEHEADER));
	bitmapv5_header = malloc(sizeof(BITMAPV5HEADER));        

	fp = fopen("/tmp/head","w");
	f = fopen("/tmp/pic.bmp", "r");
	ret = fread ( bitmap_file_header, 1, sizeof(BITMAPFILEHEADER), f);
	ret = fwrite ( bitmap_file_header, sizeof(BITMAPFILEHEADER),1, fp);
	printf("bfSize:%d,bfOffBits:%d\n",bitmap_file_header->bfSize,bitmap_file_header->bfOffBits);
	ret = fread ( bitmapv5_header, 1, sizeof(BITMAPV5HEADER), f);
	ret = fwrite ( bitmapv5_header, sizeof(BITMAPV5HEADER),1, fp);
	printf("bV5Width:%d,bV5Height:%d,bV5SizeImage:%d\n",bitmapv5_header->bV5Width,bitmapv5_header->bV5Height,bitmapv5_header->bV5SizeImage);
	fclose(fp);


	rect.x = 0;
	rect.y = 0;
	rect.width = bitmapv5_header->bV5Width;
	rect.height = bitmapv5_header->bV5Height;

	printf("fread\n");

        pSrcData = malloc(bitmapv5_header->bV5SizeImage);
	//memcpy(pSrcData,(BYTE*)refImage,IMG_WIDTH * IMG_HEIGHT * FORMAT_SIZE);
        //pSrcData = (BYTE*)refImage;
	ret = fread ( pSrcData, bitmapv5_header->bV5SizeImage, 1, f);
        fclose(f);

	f = fopen("/tmp/rgba.data", "w");
	ret = fwrite ( pSrcData, bitmapv5_header->bV5SizeImage , 1, f);
        fclose(f);

        printf("fwrite\n");

	if (!(rfx_compose_message(context, s, &rect, 1, pSrcData, rect.width, rect.height, stride)))
	{
		printf("compose message failed\n");
		goto fail;
	}
	else
	{
		printf("compose message successed\n");
	}

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
