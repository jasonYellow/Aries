#include "rfx_encode.h"

int main()
{
	RFX_CONTEXT* context = NULL;
	RFX_MESSAGE* message = NULL;
	RFX_RECT rect;
	BYTE* pSrcData = NULL;
	
	int scanline = 0;

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

	rect.x = 0;
	rect.y = 0;
	rect.width = 64;
	rect.height = 64;

        pSrcData = calloc(IMG_WIDTH * IMG_HEIGHT , FORMAT_SIZE);
	memcpy(pSrcData,(BYTE*)refImage,IMG_WIDTH * IMG_HEIGHT * FORMAT_SIZE);
        //pSrcData = (BYTE*)refImage;

	scanline = ALIGN_SCREEN_SIZE(64, 4) * 4;

	if (!(message = rfx_encode_message(context, &rect, 1, pSrcData, 1920, 1080, scanline)))
	{
		printf("rfx encode message failded\n");
		goto fail;
	}
	else
	{
		printf("rfx encode message successed\n");
	}

	printf("YLen:%d,CbLen:%d,CrLen:%d\n",message->tiles[0]->YLen,message->tiles[0]->CbLen,message->tiles[0]->CrLen);
	FILE *f = NULL;
	int ret = 0;
	f = fopen("/tmp/YCbCr.data", "w");
	ret = fwrite(message->tiles[0]->YData, message->tiles[0]->YLen , 1 , f);
	//printf("YData fwrite ret=%d\n",ret);
	//fclose(f);

	//f = fopen("/tmp/Cb.data", "w");
        ret = fwrite(message->tiles[0]->CbData, message->tiles[0]->CbLen , 1 , f);
        //printf("CrData fwrite ret=%d\n",ret);
        //fclose(f);

	//f = fopen("/tmp/Cr.data", "w");
        ret = fwrite(message->tiles[0]->CrData, message->tiles[0]->CrLen , 1 , f);
        //printf("CrData fwrite ret=%d\n",ret);
        fclose(f);

fail:
	rfx_message_free(context, message);
	rfx_context_free(context);
	return 0;
}
