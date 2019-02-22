#ifndef INCLUDED_HANDLE_REQUEST_H
#define INCLUDED_HANDLE_REQUEST_H

void accept_request(int fd);
/***********************************************
 * 这个函数每次接收一行，一行是指以'\r','\n'或
 * '\r\n'结束的字符串，并将每行的换行标志替换成
 * '\n'，且在每个字符串末尾加上字符'\0'。
 * 参数：socket 文件描述符
 *       接收字符串的缓冲区
 *       缓冲区大小
 * 返回值：读到的字节数（不包括空字符）
 * *********************************************/
int get_line(int fd, char *buffer, int size);

#endif
