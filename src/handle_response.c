#include "handle_response.h"
#include "handle_request.h"
#include "mime_type.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/wait.h>

void response_header(int sockfd, const char *filename)
{
    int response_header_file = -1;
    int nums_read = -1;
    char buf[1024];
    char rfilename[512];

    sprintf(rfilename, "../response_header/%s", filename);
    printf("%s\n", rfilename);
    response_header_file = open(rfilename, O_RDONLY);
    if (response_header_file == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    while ( (nums_read = read(response_header_file, buf, sizeof(buf)) > 0 )) {
        send(sockfd, buf, strlen(buf), 0);
        printf("%s", buf);
    }
    close(response_header_file);
}

void cat(int sockfd, FILE *file)
{
#define BUFFER_SIZE 1024
    char buffer[BUFFER_SIZE];
    int length = 0;
    // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止
    while((length = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0)
    {
        send(sockfd, buffer, length, 0);
    }
}

void success_header(int sockfd, const char *path)
{
    const char *mime_type = NULL;
    char buf[1024];
    char time_buf[100];
    time_t now;
    struct stat st;
    stat(path, &st);
    
    mime_type = get_mime_type(path);
    now = time((time_t )0);
    strftime(time_buf, sizeof(time_buf), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));
    snprintf(buf, sizeof(buf), "HTTP/1.0 200 OK\r\n\
Content-Type: %s\r\n\
Date: %s\r\n\
Connection: close\r\n\
Server: pzhttpd\r\n\
Content-Length: %lld\r\n\r\n", mime_type, time_buf, (long long)st.st_size);
    send(sockfd, buf, strlen(buf), 0);
}

void handle_GET(int sockfd, char *path)
{
    FILE *resource = NULL;
    int num_chars = -1;
    char buf[1024]; 
    /* 丢弃所有未读的消息(headers） */
    do {
        num_chars = get_line(sockfd, buf, sizeof(buf));
    } while ( (num_chars > 0) && (strcmp("\n", buf)) );

    resource = fopen(path, "rb");
    if (resource == NULL) {
        response_header(sockfd, "404");
        return;
    } else {
        success_header(sockfd, path); /* 写入 GET 请求成功时的响应头信息 */
        cat(sockfd, resource); /* 写入请求文件信息 */
        fclose(resource);
    }
}

void handle_POST(int sockfd, char *path)
{
    int num_chars;
    int content_length = -1;
    char buf[1024];
    int cgi_output[2];
    int cgi_input[2];
    pid_t pid;
    int status;
    char c;

    /* 获取 Content-length: */
    num_chars = get_line(sockfd, buf, sizeof(buf));
    while ( (num_chars > 0) && (strcmp("\n", buf))) {
        buf[15] = '\0';
        if (strcasecmp(buf, "Content-Length:") == 0) {
            content_length = atoi(&buf[16]);
        }
        num_chars = get_line(sockfd, buf, sizeof(buf));
    }
    if (content_length == -1) {
        response_header(sockfd, "400");
        return;
    }
    /* 建立管道 */
    if (pipe(cgi_output) < 0) {
        response_header(sockfd, "500");
        return;
    }
    if (pipe(cgi_input) < 0) {
        response_header(sockfd, "500");
        return;
    }

    if ((pid = fork()) < 0) {
        response_header(sockfd, "500");
        return;
    }
    if ( pid == 0 ) { /* 子进程 */
        char meth_env[255];
        char query_env[255];
        char length_env[255];
 
        /* 把 STDOUT 重定向到 cgi_output 的写入端 */
        dup2(cgi_output[1], 1);
        /* 把 STDIN 重定向到 cgi_input 的读取端 */
        dup2(cgi_input[0], 0);
        /* 关闭 cgi_input 的写入端 和 cgi_output 的读取端 */
        close(cgi_output[0]);
        close(cgi_input[1]);
        /*设置 request_method 的环境变量*/
        sprintf(meth_env, "REQUEST_METHOD=POST");
        putenv(meth_env);
        /*设置 content_length 的环境变量*/
        sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
        putenv(length_env);
        
        /*用 execl 运行 cgi 程序*/
        execl(path, path, NULL);
        exit(0);
    } else {    /* parent */
        /* 关闭 cgi_input 的读取端 和 cgi_output 的写入端 */
        close(cgi_output[1]);
        close(cgi_input[0]);
        /*接收 POST 过来的数据*/
        for (int i = 0; i < content_length; i++) {
            recv(sockfd, &c, 1, 0);
            /*把 POST 数据写入 cgi_input，现在重定向到 STDIN */
            write(cgi_input[1], &c, 1);
        }
        /* 正确，HTTP 状态码 200 */ 
        sprintf(buf, "HTTP/1.0 200 OK\r\n");
        send(sockfd, buf, strlen(buf), 0);
        /*读取 cgi_output 的管道输出到客户端，该管道输入是 STDOUT */
        while (read(cgi_output[0], &c, 1) > 0)
            send(sockfd, &c, 1, 0);
 
        /*关闭管道*/
        close(cgi_output[0]);
        close(cgi_input[1]);
        /*等待子进程*/
        waitpid(pid, &status, 0);
    }
}
void handle_HEAD(int sockfd, char *path)
{
    success_header(sockfd, path);
}
