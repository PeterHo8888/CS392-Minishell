/*******************************************************************************
 * Name        : builtins.h
 * Author      : Peter Ho
 * Date        : Apr 14, 2021
 * Description : Header file
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#pragma once

extern void init_builtins();
extern int (*get_builtin(const char []))(int, char *[]);
