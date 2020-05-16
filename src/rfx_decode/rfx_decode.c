#include "rfx_decode.h"
#include "stdio.h"
#include <winpr/crt.h>
#include <winpr/tchar.h>
#include <winpr/path.h>
#include <winpr/file.h>
#include <winpr/wlog.h>

#define FORMAT PIXEL_FORMAT_XRGB32 

static BYTE encodeHeaderSample[] = {
	/* as in 4.2.2 */
	0xc0, 0xcc, 0x0c, 0x00, 0x00, 0x00, 0xca, 0xac, 0xcc, 0xca, 0x00, 0x01, 0xc3, 0xcc, 0x0d, 0x00,
	0x00, 0x00, 0x01, 0xff, 0x00, 0x40, 0x00, 0x28, 0xa8, 0xc1, 0xcc, 0x0a, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x01, 0xc2, 0xcc, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00
};

static BYTE dataSampleHead[] = {
	/* FRAME_BEGIN as in 4.2.3 */
	0xc4, 0xcc, 0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,

	/* REGION as in 4.2.3 */
	0xc6, 0xcc, 0x17, 0x00, 0x00, 0x00, 0x01, 0x00, 0xcd, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
	0x00, 0x40, 0x00, 0xc1, 0xca, 0x01, 0x00,

	/*TILESET HEADER*/
	0xc7, 0xcc, 0x39, 0x0b, 0x00, 0x00, 0x01, 0x00, 0xc2, 0xca, 0x00, 0x00, 0x51, 0x50, 0x01, 0x40,
        0x01, 0x00, 0x23, 0x0b, 0x00, 0x00, 0x66, 0x66, 0x77, 0x88, 0x98, 0xc3, 0xca, 0x23, 0x0b, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xad, 0x03, 0xcd, 0x03, 0x91, 0x03
};

static BYTE dataSampleEnd[] = {
/* FRAME_END as in 4.2.3 */
	0xc5, 0xcc, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00
};

int main()
{
	int rc = -1;
	int ret = 0;
	int dataHeadLen = 0;
	int dataEndLen = 0;
	int dataTotalLen = 0;
	FILE *fp;
	REGION16 region = { 0 };
	RFX_CONTEXT* context = NULL;
        BYTE* dest = NULL;
	BYTE* encodeDataSample = NULL;
	size_t stride = FORMAT_SIZE * IMG_WIDTH;
	
	context = rfx_context_new(FALSE);
	if (!context)
	{
	    printf("rfx context new failed\n");
	    goto fail;
	}
	else
	    printf("rfx context new successed\n");

	dest = calloc(IMG_WIDTH * IMG_HEIGHT, FORMAT_SIZE);
	if(!dest)
	{
		printf("dest calloc failed\n");
		goto fail;
	}
	else
		printf("dest calloc successed\n");
	
	dataHeadLen = sizeof(dataSampleHead);
	dataEndLen = sizeof(dataSampleEnd);
	dataTotalLen = dataHeadLen + 2827 + dataEndLen;
	printf("dataHeadLen=%d,dataEndLen=%d,dataTotalLen:%d\n",dataEndLen,dataHeadLen,dataTotalLen);

	encodeDataSample = malloc(dataTotalLen);
	memcpy(encodeDataSample,dataSampleHead,dataHeadLen);

	fp = fopen("/tmp/YCbCr.data", "r");
	ret = fread(&encodeDataSample[dataHeadLen],1,2827,fp);
	fclose(fp);

	memcpy(&encodeDataSample[dataHeadLen + 2827],dataSampleEnd,dataEndLen);

	fp = fopen("/tmp/encode.data", "w");
        ret = fwrite(encodeDataSample,dataTotalLen , 1 , fp);
	fclose(fp);

	region16_init(&region);
	if (!rfx_process_message(context, encodeHeaderSample, sizeof(encodeHeaderSample), 0, 0, dest,
	                     	FORMAT, stride, IMG_HEIGHT, &region))
	{
		printf("process encodeHeaderSample failed\n");
		goto fail;
	}
	else
	{
		printf("process encodeHeaderSample successed\n");
	}

	region16_clear(&region);

	if (!rfx_process_message(context, encodeDataSample, sizeof(encodeDataSample), 0, 0, dest,
	                        FORMAT, stride, IMG_HEIGHT, &region))
	{
		printf("process encodeDataSample failed\n");
		goto fail;
	}
	else
		printf("process encodeDataSample successed\n");

	region16_print(&region);

        printf("decode successed\n");
	fp = fopen("/tmp/decode.data", "w");
	fwrite(dest, IMG_WIDTH * IMG_HEIGHT, FORMAT_SIZE, fp);
	fclose(fp);

fail:
	free(dest);
	rfx_context_free(context);

	return 0;
}
