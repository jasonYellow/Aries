#include <winpr/crt.h>
#include <winpr/print.h>

#include <freerdp/freerdp.h>
#include <freerdp/codec/rfx.h>
#include <sys/time.h>

#define IMG_WIDTH 1920
#define IMG_HEIGHT 1080
#define FORMAT_SIZE 4
#define FORMAT PIXEL_FORMAT_BGRA32

#define IMG_INDEX  12
#define IMG_NUM 100 

#define HEAD_SIZE 54 


int main(int argc, char* argv[])
{
	int rc = -1;
	REGION16 region = { 0 };
	RFX_CONTEXT* context = NULL;
	BYTE* dest = NULL;
	BYTE* src = NULL;
	int ret = 0;
	int i;
	size_t stride = FORMAT_SIZE * IMG_WIDTH;
	struct timeval start,end;
	int interval=0;
	char   *filename;

	filename = malloc(100);

	context = rfx_context_new(FALSE);
	if (!context)
		goto fail;

	dest = calloc(IMG_WIDTH * IMG_HEIGHT, FORMAT_SIZE);
	if (!dest)
		goto fail;

	FILE *fp_encode;
	int32_t encode_size;
	fp_encode = fopen("/tmp/bitmap_encode","r");

	region16_init(&region);
	for(i=0;i<IMG_NUM;i++)
	{

		region16_clear(&region);
		ret = fread(&encode_size,1,sizeof(encode_size),fp_encode);
		printf("encode_size[%d]:%d\n",i,encode_size);

		src = malloc(encode_size);
		ret = fread(src,1,encode_size,fp_encode);

		gettimeofday(&start,NULL);
        	if (!rfx_process_message(context, src, encode_size, 0, 0, dest,
                                 FORMAT, stride, IMG_HEIGHT, &region))
                	goto fail;

        	gettimeofday(&end,NULL);
#if 1 
		if(i%10 == 0)
		{
			sprintf(filename,"/tmp/decode_%d.bmp",i/10);
			
			FILE *fp = fopen(filename, "wb");
        		FILE *fp_head = fopen("/tmp/headv1", "rb");
       			BYTE *headv1 = malloc(HEAD_SIZE);
        		ret = fread(headv1,1,HEAD_SIZE,fp_head);
        		ret = fwrite(headv1, HEAD_SIZE, 1, fp);
        		ret = fwrite(dest, IMG_WIDTH * IMG_HEIGHT, FORMAT_SIZE, fp);
        		fclose(fp);
        		fclose(fp_head);
		}
#endif
		interval += (end.tv_sec-start.tv_sec)*1000+(end.tv_usec-start.tv_usec)/1000;
		free(src);
	}

	printf("aver interval:%d\n",interval/IMG_NUM);


fail:
	region16_uninit(&region);
	rfx_context_free(context);
	free(dest);
	return rc;
}
