#include "mime_type.h"

#include <string.h>
#include <stdio.h>

const MIME_TYPE MIMES[] = {
    {NULL, "text/plain; charset=UTF-8"}, // default type
    {".html", "text/html; charset=UTF-8"},
    {".htm", "text/html; charset=UTF-8"},
    {".xhtml", "application/xhtml+xml; charset=UTF-8"},
    {".xht", "application/xhtml+xml; charset=UTF-8"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif", "image/gif"},
    {".png", "image/png"},
    {".ico", "image/x-icon"},
    {".css", "text/css"},
    {".xml", "text/xml; charset=UTF-8"},
    {".xsl", "text/xml; charset=UTF-8"},
    {".au", "audio/basic"},
    {".wav", "audio/wav"},
    {".avi", "video/x-msvideo"},
    {".mov", "video/quicktime"},
    {".qt", "video/quicktime"},
    {".mpeg", "video/mpeg"},
    {".mpe", "video/mpeg"},
    {".vrml", "model/vrml"},
    {".wrl", "model/vrml"},
    {".midi", "audio/midi"},
    {".mid", "audio/midi"},
    {".mp3", "audio/mpeg"},
    {".ogg", "application/ogg"},
    {".pac", "application/x-ns-proxy-autoconfig"},
};

const char *get_mime_type(const char *path)
{
    char *dot = NULL;
    int i = 1;

    dot = strrchr(path, '.');
    if (dot == NULL) {
        return MIMES[0].type;
    }
    for (i = 1; i < sizeof(MIMES) / sizeof(MIME_TYPE); i++) {
        if (strcmp(dot, MIMES[i].suffix) == 0)
            return MIMES[i].type;
    }
    printf("2121\n");
    return MIMES[0].type;
}
