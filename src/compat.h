#ifndef INTERPLAY_COMPAT_H_
#define INTERPLAY_COMPAT_H_

#include <stdio.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

FILE *open_file(const char *path);

#if defined(_WIN32) || defined(WIN32)
#define scan_line sscanf_s
#else
#define scan_line sscanf
#endif

#endif  // INTERPLAY_COMPAT_H_
