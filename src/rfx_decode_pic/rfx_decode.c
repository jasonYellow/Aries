#include <winpr/crt.h>
#include <winpr/print.h>

#include <freerdp/freerdp.h>
#include <freerdp/codec/rfx.h>

#define IMG_WIDTH 1920
#define IMG_HEIGHT 1080
#define FORMAT_SIZE 4
#define FORMAT PIXEL_FORMAT_BGRA32

int main(int argc, char* argv[])
{
	int rc = -1;
	REGION16 region = { 0 };
	RFX_CONTEXT* context = NULL;
	BYTE* dest = NULL;
	BYTE* src = NULL;
	int ret = 0;
	size_t stride = FORMAT_SIZE * IMG_WIDTH;

	context = rfx_context_new(FALSE);
	if (!context)
		goto fail;

	dest = calloc(IMG_WIDTH * IMG_HEIGHT, FORMAT_SIZE);
	if (!dest)
		goto fail;

	FILE *f;
	f = fopen("/tmp/encode.data","r");
	ret = fseek(f,0L,SEEK_END);  
    	int size=ftell(f);
	printf("file len:%d\n",size);

	src = malloc(size);

	fseek(f,0,SEEK_SET);
	ret = fread(src,1,size,f);
	fclose(f);

	region16_init(&region);
	if (!rfx_process_message(context, src, size, 0, 0, dest,
	                         FORMAT, stride, IMG_HEIGHT, &region))
		goto fail;
	region16_print(&region);

	printf("process message successed\n");

#if 1
	FILE *fp = fopen("/tmp/decode.bmp", "w");
	FILE *fp_head = fopen("/tmp/head", "r");
	ret = fseek(fp_head,0L,SEEK_END);
        int size_head=ftell(fp_head);
        printf("head len:%d\n",size_head);
	fseek(fp_head,0,SEEK_SET);
	BYTE *head = malloc(size_head);
	fread(head,1,size_head,fp_head);
	fwrite(head, size_head, 1, fp);
	fwrite(dest, IMG_WIDTH * IMG_HEIGHT, FORMAT_SIZE, fp);
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
