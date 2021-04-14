/*******************************************************************************
 * Name        : util.h
 * Author      : Peter Ho
 * Date        : Apr 14, 2021
 * Description : Utility macros
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#pragma once

#define ARGS_MAX 256
#define ARG_LEN 65536

//#define DEBUG
//#define NOSTDIO

#ifdef DEBUG
#include <stdio.h>
#define debugln(fmt, ...) \
    do { \
        fprintf(stderr, "minishell: "fmt"\n", ##__VA_ARGS__); \
        fflush(stderr); \
    } while (0)
#else
#define debugln(fmt, ...) (void)(fmt)
#endif

#ifdef NOSTDIO

#define ERROR(fmt, ...) do {} while (0)
#define print(fmt, ...) do {} while (0)

#else

#define ERROR(fmt, ...) \
    do { \
        fprintf(stderr, "Error: "fmt"\n", ##__VA_ARGS__); \
        fflush(stderr); \
    } while (0)

#define print(fmt, ...) \
    do { \
        fprintf(stdout, fmt, ##__VA_ARGS__); \
        fflush(stdout); \
    } while (0)

#endif

#define UNUSED(x) (void)(x)
