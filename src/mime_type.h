#ifndef INCLUDED_MIME_TYPE_H
#define INCLUDED_MIME_TYPE_H

#include <stdio.h>

typedef struct MIME_TYPE
{
    const char *suffix;
    const char *type;
} MIME_TYPE;
 
/*************************************************
 * 这个函数根据文件路径返回资源类型
 * 参数：文件路径
 * 返回值：资源类型
 * ************************************************/
const char *get_mime_type(const char *path);
#endif
