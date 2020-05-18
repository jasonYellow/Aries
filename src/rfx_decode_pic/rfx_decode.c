#include <winpr/crt.h>
#include <winpr/print.h>

#include <freerdp/freerdp.h>
#include <freerdp/codec/rfx.h>

#define IMG_WIDTH 64
#define IMG_HEIGHT 64
#define FORMAT_SIZE 4
#define FORMAT PIXEL_FORMAT_XRGB32

static INLINE size_t fuzzyCompare(BYTE b1, BYTE b2)
{
	if (b1 > b2)
		return b1 - b2;
	return b2 - b1;
}

static BOOL fuzzyCompareImage(const UINT32* refImage, const BYTE* img, size_t npixels)
{
	size_t i;
	size_t totalDelta = 0;

	for (i = 0; i < npixels; i++, refImage++)
	{
		BYTE A = *img++;
		BYTE R = *img++;
		BYTE G = *img++;
		BYTE B = *img++;
		size_t delta;

		if (A != 0x00)
			return FALSE;

		delta = fuzzyCompare(R, (*refImage & 0x00ff0000) >> 16);
		if (delta > 1)
			return FALSE;
		totalDelta += delta;

		delta = fuzzyCompare(G, (*refImage & 0x0000ff00) >> 8);
		if (delta > 1)
			return FALSE;
		totalDelta += delta;

		delta = fuzzyCompare(B, (*refImage & 0x0000ff));
		if (delta > 1)
			return FALSE;
		totalDelta += delta;
	}

	WLog_DBG("test", "totalDelta=%d (npixels=%d)", totalDelta, npixels);
	return TRUE;
}

int main(int argc, char* argv[])
{
	int rc = -1;
	REGION16 region = { 0 };
	RFX_CONTEXT* context = NULL;
	BYTE* dest = NULL;
	BYTE* src = NULL;
	size_t stride = FORMAT_SIZE * 256;

	context = rfx_context_new(FALSE);
	if (!context)
		goto fail;

	dest = calloc(256 * 256, FORMAT_SIZE);
	if (!dest)
		goto fail;

	FILE *f;
	f = fopen("/tmp/encode.data","r");
	fseek(f,0L,SEEK_END);  
    	int size=ftell(f);
	printf("file len:%d\n",size);

	src = malloc(size);

	fseek(f,0,SEEK_SET);
	fread(src,1,size,f);
	fclose(f);

	region16_init(&region);
	if (!rfx_process_message(context, src, size+256, 0, 0, dest,
	                         FORMAT, stride, 256, &region))
		goto fail;
	region16_print(&region);

	printf("process message successed\n");

#if 1
	FILE *fp = fopen("/tmp/windows.data", "w");
	fwrite(dest, 256 * 256, FORMAT_SIZE, fp);
	fclose(fp);
#endif
/*
	if (!fuzzyCompareImage(refImage, dest, IMG_WIDTH * IMG_HEIGHT))
	{
		printf("compare failed\n");
		goto fail;
	}
	else
	{
		printf("compare successed\n");
	}
	rc = 0;
*/
fail:
	region16_uninit(&region);
	rfx_context_free(context);
	free(dest);
	return rc;
}
