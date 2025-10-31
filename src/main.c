#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { log_info, log_error, log_fatal } log_severity;

void logmsg(log_severity severity, const char* format, ...)
{
    const char* red = "\x1b[31m";
    const char* cyan = "\x1b[36m";
    const char* reset = "\x1b[0m";
    printf("%s", severity == log_info ? cyan : red);

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);

    printf("\n%s", reset);
    if (severity == log_fatal)
        exit(-1);
}

int main()
{
    logmsg(log_info, "%d!", 10);
}
