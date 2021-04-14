#/*******************************************************************************  \
 * Name        : Makefile                                                          \
 * Author      : Peter Ho                                                          \
 * Date        : Apr 14, 2021                                                      \
 * Description : Makefile for Assignment 5, Minishell                              \
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System. \
 ******************************************************************************/
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic-errors -O3 -march=native -mtune=native

BUILD_DIR = build
APP = minishell

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

$(APP): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

-include $(BUILD_DIR)/*.d

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(APP)
