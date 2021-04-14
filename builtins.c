/*******************************************************************************
 * Name        : builtins.c
 * Author      : Peter Ho
 * Date        : Apr 14, 2021
 * Description : Builtins implementations and helpers
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include "builtins.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <linux/limits.h>

#define BUILTINS_MAX 256

static size_t builtins_size;
static int (*builtins[BUILTINS_MAX])(int argc, char *argv[]);
static const char *builtins_str[BUILTINS_MAX];

static void register_builtin(const char cmd[], int (*fn)(int, char *[]))
{
    if (builtins_size == BUILTINS_MAX) {
        ERROR("Cannot register builtin %s. Builtins limit exceeded.", cmd);
        return;
    }

    builtins_str[builtins_size] = cmd;
    builtins[builtins_size] = fn;
    debugln("Builtin %s registered.", cmd);

    ++builtins_size;
}

// Future optimization can be done with a hashmap, but the number of builtins
// is so small that correctness can be valued more here
int (*get_builtin(const char cmd[]))(int, char *[])
{
    if (cmd != NULL) {
        for (size_t i = 0; i < builtins_size; ++i)
            if (!strcmp(cmd, builtins_str[i]))
                return builtins[i];
    }
    return NULL;
}

static int ___builtin_cd(int argc, char *argv[])
{
    if (argc > 2) {
        ERROR("Too many arguments to cd.");
        return EXIT_SUCCESS;
    }

    char *input = (argc == 1) ? "~" : argv[1];
    char target[PATH_MAX] = {0};

    if (input[0] == '~') {
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        if (pw) {
            strcat(target, pw->pw_dir);
            strcat(target, input + 1);
        } else {
            ERROR("Cannot get passwd entry. %s.", strerror(errno));
        }
    } else {
        strcpy(target, argv[1]);
    }

    if (chdir(target) == -1)
        ERROR("Cannot change directory to '%s'. %s.", target, strerror(errno));

    return EXIT_SUCCESS;
}

static int ___builtin_exit(int argc, char *argv[])
{
    UNUSED(argv);
    UNUSED(argc);
    return EXIT_FAILURE;
}

static int ___builtin_about(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    print("Minishell, a Minimum Shell implementation\n"
            "Author: Peter Ho\n"
            "Class: CS 392\n"
            "Build date: " __DATE__ "\n"
          );
    return EXIT_SUCCESS;
}

void init_builtins()
{
    register_builtin("cd", ___builtin_cd);
    register_builtin("exit", ___builtin_exit);
    register_builtin("about", ___builtin_about);
}
