#ifndef _ARDUINO_COMPAT_H
#define _ARDUINO_COMPAT_H

#include <stdio.h>
#include <stdlib.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>

#include <ctype.h>

#include <string.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef bool boolean;
//#define uint8_t unsigned char

#define FILE_READ O_RDONLY
#define FILE_WRITE (O_RDWR | O_CREAT)

#endif
