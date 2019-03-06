#include "handle_request.h"
#include "handle_response.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int get_line(int fd, char *buffer, int size)
{
    char c = '\0';
    int i = 0;
    int n = -1;

    while ( (i < size - 1) && (c != '\n') ) {
        n = recv(fd, &c, 1, 0); /* 每次接收一个字符 */
        if (n > 0) {
            if (c == '\r') {
                n = recv(fd, &c, 1, MSG_PEEK); /* 不将接收到的字符从接收队列中移除 */
                if ( (n > 0) && (c == '\n') )
                    /* 回车换行符 "\r\n" */
                    if (recv(fd, &c, 1, 0) < 0) {
                        perror("recv");
                        exit(EXIT_FAILURE);
                    } 
                else
                    c = '\n'; 
            }
            buffer[i++] = c;
        } else
            break; /* 没有数据读取 */
    }
    buffer[i] = '\0';
    return i;
}

int is_safe_url(const char *url)
{
    printf("2222222222\n");
    const char *file = url + 1;
    int len = strlen(file);
    printf("file: %s\n", file);
    if (file[0] == '/' || 
            strcmp(file, "..") == 0 || 
            strncmp(file, "../", 3) == 0 || 
            strstr(file, "/../") != NULL || 
            ( len > 3 && strcmp(&(file[len - 3]), "/..") == 0) ) {
        return 0;
    }
    return 1;
}

#define LINESIZE 1024 /* 每行最多 1024 字节 */
#define METHODSIZE 255
#define URLSIZE 255
const char *methods[] = {
    "GET",
    "POST",
    "HEAD",
};
typedef void (*ResponseFunc)(int, char *);
ResponseFunc response_func[] = {
    &handle_GET,
    &handle_POST,
    &handle_HEAD,
};
void accept_request(int fd)
{
    char buf[LINESIZE];
    int num_chars = -1;
    char method[METHODSIZE];
    char url[URLSIZE];
    char path[512];
    int i = 0, j = 0;
    int nums_methods = sizeof(methods) / sizeof(const char *);
    int implement_flag = 0;
    int method_index = -1;
    ResponseFunc response = NULL;

    num_chars = get_line(fd, buf, LINESIZE);
    /* 解析 http 请求报文，获取请求方法 */
    i = 0; j = 0; /* j 记录空格出现的位置，为提取 url 准备 */
    while ( (!isspace((int)buf[j])) && (i < METHODSIZE - 1) &&
            (j < LINESIZE) ) {
        method[i] = buf [j];
        i++; j++;
    }
    method[i] = '\0';
    /* 判断该求请求方法是否实现 */
    for (i = 0; i < nums_methods; ++i) {
        if (!strcasecmp(method, methods[i])) {
            implement_flag = 1;
            method_index = i;
            break;
        }
    } 
    if (!implement_flag) {
        response_header(fd, "501");
        return;
    }
    /* 获取 url */
    i = 0; 
    while ( (isspace((int)buf[j])) && (j < LINESIZE) )
        j++;
    while ( (!isspace((int)buf[j])) && (i < URLSIZE - 1) && 
            (j < LINESIZE) ) {
        url[i] = buf[j];
        i++; j++;
    }
    url[i] = '\0';
    
    if (!is_safe_url(url)) {
        response_header(fd, "400");
        return;
    }

    /* 格式化 url 到 path 数组，html 文件都在 htdocs 中 */
    sprintf(path, "../htdocs%s", url);
    /*默认情况为 index.html */
    if (path[strlen(path) - 1] == '/')
        strcat(path, "index.html");

    response = response_func[method_index];
    response(fd, path);
}
