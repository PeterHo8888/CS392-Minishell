/*******************************************************************************
 * Name        : parse.c
 * Author      : Peter Ho
 * Date        : Apr 14, 2021
 * Description : Parsing logic for Minishell
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define is_space(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r')

ssize_t parse(char *args[], int *err, int *argc)
{
    static char buf[ARG_LEN];

    ssize_t bytes_read = read(STDIN_FILENO, buf, sizeof(buf) - 1);
    if (bytes_read <= 0)
        return bytes_read;

    enum {REG, QUOTE, WHITE} state = WHITE;

    char word[ARG_LEN] = {0};

    size_t word_idx = 0, arg_idx = 0;

    // This is correct.  bytes_read is at most ARG_LEN - 1.
    buf[bytes_read] = '\0';

    // Can be made more efficient by using pointers and memcpy
    const char *s = buf;
    while (*s && arg_idx < ARGS_MAX) {
        char c = *s;
        switch (state) {
        case REG:
            // Space following a regular, unquoted char terminates string
            if (is_space(c)) {
                word[word_idx++] = '\0';
                memcpy(args[arg_idx++], word, word_idx);
                state = WHITE;
            } else if (c == '\"') {
                // Something like /root/"Document and Settings"
                state = QUOTE;
            } else {
                word[word_idx++] = c;
            }
            break;
        case QUOTE:
            if (c == '\"')
                state = REG;
            else
                word[word_idx++] = c;
            break;
        case WHITE:
            if (c == '\"') {
                word_idx = 0;
                state = QUOTE;
            } else if (!is_space(c)) {
                word_idx = 0;
                word[word_idx++] = c;
                state = REG;
            }
            break;
        }
        ++s;
    }

    // reading e.g. a file and it isn't whitespace terminated
    if (state == REG) {
        word[word_idx++] = '\0';
        memcpy(args[arg_idx++], word, word_idx);
    }

    /*
    debugln("end state: %s",
            state == WHITE ? "WHITE"
            : state == QUOTE ? " QUOTE"
            : "REG"
           );
    */

    if (state == QUOTE) {
        ERROR("Malformed command.");
        *err = 1;
    }

    *argc = arg_idx;

    return bytes_read;
}
