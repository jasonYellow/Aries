#ifndef RFX_ENCODE_H
#define RFX_ENCODE_H

#include <stdio.h>

#include <winpr/crt.h>
#include <winpr/print.h>

#include <freerdp/freerdp.h>
#include <freerdp/codec/rfx.h>

#define IMG_WIDTH 64
#define IMG_HEIGHT 64
#define FORMAT_SIZE 4

#define ALIGN_SCREEN_SIZE(size, align) ((size + align - 1) & (~(align - 1)))

typedef unsigned char BYTE;

#endif
