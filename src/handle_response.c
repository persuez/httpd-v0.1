#include "handle_response.h"
#include "handle_request.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

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
    char buf[1024];

    fgets(buf, sizeof(buf), file);
    while (!feof(file)) {
        send(sockfd, buf, strlen(buf), 0);
        printf("1%s", buf);
        fgets(buf, sizeof(buf), file);
    }
}

void handle_GET(int sockfd, char *path)
{
    FILE *resource = NULL;
    int num_chars = -1;
    char buf[1024]; 
    /* 丢弃所有未读的消息(headers） */
    do {
        num_chars = get_line(sockfd, buf, sizeof(buf));
        printf("2.%s", buf);
    } while ( (num_chars > 0) && (strcmp("\n", buf)) );

    resource = fopen(path, "r");
    if (resource == NULL) {
        response_header(sockfd, "404");
        return;
    } else {
        response_header(sockfd, "200"); /* 写入响应头信息 */
        cat(sockfd, resource); /* 写入请求文件信息 */
        fclose(resource);
    }
}
