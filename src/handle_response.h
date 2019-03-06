#ifndef INCLUDED_HANDLE_RESPONSE_H
#define INCLUDED_HANDLE_RESPONSE_H

#include <stdio.h>

/***********************************************
 * 这个函数响应 GET 请求
 * 参数：服务器 socket 文件描述符
 *       请求路径
 * 没有返回值
 * **********************************************/
void handle_GET(int sockfd, char *path);
/************************************************
 * 根据文件名提供相对应的响应头信息
 * 参数：socket 文件描述符
 *       文件名（以状态码命名）
 * 无返回值
 * ***********************************************/
void handle_POST(int sockfd, char *path);
void handle_HEAD(int sockfd, char *path);
void response_header(int soctfd, const char *filename);
/************************************************
 * 这个函数将请求文件写到 sockfd 中
 * 参数：sockfd
 *       file(FILE *)
 * 没有返回值
 * ***********************************************/
void cat(int sockfd, FILE *file);
/*************************************************
 * 这个函数在请求返回 200 状态码时调用，写入
 * 响应头部（除起始行外）信息
 * 参数：sockfd
 *       请求资源路径
 * 没有返回值
 * ************************************************/
void success_header(int sockfd, const char *path);

#endif
