/*******************************************************************************
 * Name        : minishell.c
 * Author      : Peter Ho
 * Date        : Apr 14, 2021
 * Description : Homework 5 - Minishell in C
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include "builtins.h"
#include "parse.h"
#include "util.h"
#include "colors.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/wait.h>

static volatile sig_atomic_t ctrl_c;

static void sig_handler(int);
static int minishell();

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        ERROR("Cannot register signal handler. %s.", strerror(errno));
        return EXIT_FAILURE;
    }

    init_builtins();

    return minishell();
}

void sig_handler(int sig)
{
    UNUSED(sig);
    print("\n");
    ctrl_c = 1;
}

int run(int argc, char *args[])
{
    // builtin exit is the only way that run() returns EXIT_FAILURE
    int (*builtin_fn)(int, char *[]) = (void *)0;

    if ((builtin_fn = get_builtin(args[0])))
        return builtin_fn(argc, args);

    pid_t pid = fork();
    if (pid < 0) {
        ERROR("fork() failed. %s.", strerror(errno));
    } else if (pid > 0) {
        while (1) {
            pid = wait(NULL);
            if (pid == -1 && errno != EINTR) {
                ERROR("wait() failed. %s.", strerror(errno));
                break;
            } else if (pid > -1) {
                break;
            }
        }
    } else {
        execvp(args[0], args);
        ERROR("exec() failed. %s.", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

int minishell()
{
    static char *args_pool;
    static char *args[ARGS_MAX];
    int status = EXIT_SUCCESS;

    if (!(args_pool = malloc(ARG_LEN * ARGS_MAX))) {
        ERROR("malloc() failed. %s.", strerror(errno));
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < ARGS_MAX; ++i)
        args[i] = args_pool + (i * ARG_LEN);

    while (1) {
        ctrl_c = 0;
        char cwd[PATH_MAX];
        if (!getcwd(cwd, sizeof(cwd))) {
            ERROR("Cannot get current working directory. %s.", strerror(errno));
            status = EXIT_FAILURE;
            break;
        }

        print(DEFAULT "[" BRIGHTBLUE "%s" DEFAULT "]$ ", cwd);

        // err is only used for malformed input
        // argc is num of args
        // args doesn't need to be cleared, we overwrite in parse()
        int err = 0, argc;
        ssize_t bytes_read = parse(args, &err, &argc);

        //for (int i = 0; i < argc; ++i)
            //debugln("arg %d = %s", i, args[i]);

        // read() returning 0 indicates end of file (man 2 read)
        if (!bytes_read) {
            print("\n");
            debugln("Exiting due to read() returning 0");
            break;
        } else if (bytes_read < 0) {
            if (errno != EINTR) {
                ERROR("Failed to read from stdin. %s.", strerror(errno));
                status = EXIT_FAILURE;
                break;
            }
        } else {
            // only whitespace, don't run
            if (!argc)
                continue;

            // bytes_read might(?) be > 0 when ctrl-c is pressed
            // err is for malformed command
            if (!err && !ctrl_c) {
                // Save ptr and replace with nullptr
                char *save = args[argc];
                args[argc] = NULL;

                int status = run(argc, args);

                // Restore the ptr
                args[argc] = save;

                if (status != EXIT_SUCCESS)
                    break;
            }
        }
    }

    debugln("I'm freeeeeeeeeeeeeeeeee");
    free(args_pool);

    return status;
}
