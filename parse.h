/*******************************************************************************
 * Name        : parse.h
 * Author      : Peter Ho
 * Date        : Apr 14, 2021
 * Description : Header file
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#pragma once

#include <sys/types.h>
extern ssize_t parse(char *restrict args[], int *restrict err, int *restrict argc);
