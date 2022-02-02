/*
To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with
this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#define _POSIX_C_SOURCE 200809L

#include "linkedlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


enum action {
	INVALID,
	PUSH, POP,
	ADD, INSERT,
	REMOVE, REMOVE_LAST
};

struct insert_args {
	size_t index;
	char *entry;
};

struct command {
	char *filename;
	enum action action;
	void *args;
};


static void dump_usage() {
	fputs(
		"Usage: listmanip [FILE] ACTION\n"
		"  FILE\n"
		"    file to use as list; read/write to stdin/stdout when omitted\n"
		"  ACTION\n"
		"    One of the following:\n"
		"    push ENTRY...: adds entries to the front of list\n"
		"    add ENTRY...: adds entries to the end of list\n"
		"    insert INDEX ENTRY: insert entry at INDEX\n"
		"    pop [COUNT]: remove the first COUNT entries, default to 1\n"
		"    rm [INDEX]: remove last or by index\n"
		"    rmlast [COUNT]: remove the last COUNT entries, default to 1\n"
		, stderr);
}


static char *clone(const char *source) {
	char *buf = calloc(strlen(source)+1, sizeof (*buf));
	return strcpy(buf, source);
}


#define FAIL_IF(cond, msg) do { \
	if (cond) { \
		fputs((msg), stderr); \
		exit(EXIT_FAILURE); \
	} \
} while (0);

static size_t str_to_non_neg(char *s) {
	errno = 0;
	long int n = strtol(s, NULL, 10);
	FAIL_IF(errno || n < 0, "Invalid number\n");
	return n;
}


/**
 * Parses argc and argv to extract the input filename and action.
 * Checks if arguments are valid. Wraps extracted information into
 * a command struct.
 */
static struct command parse_command_line(int argc, char **argv) {

	if (argc < 2) {
		dump_usage();
		FAIL_IF(1, "Insufficient arguments\n");
	}

	struct command cmd = { .action = INVALID };

	for (size_t i = 1; cmd.action == INVALID && i < argc; ++i) {
		#define OP(s) ( strcmp(argv[i], (s)) == 0 )
		if (OP("push")) {
			cmd.action = PUSH;
			cmd.args = &argv[i+1];
		}
		else if (OP("add")) {
			cmd.action = ADD;
			cmd.args = &argv[i+1];
		}
		else if (OP("insert")) {
			cmd.action = INSERT;
			FAIL_IF(argc - (i+1) < 2, "Insufficient arguments\n");
			struct insert_args *ins_args = cmd.args =
					malloc(sizeof (struct insert_args));
			ins_args->index = str_to_non_neg(argv[++i]);
			ins_args->entry = argv[++i];
		}
		else if (OP("pop")) {
			cmd.action = POP;
			if (argv[++i]) {
				cmd.args = malloc(sizeof (size_t));
				*((size_t *) cmd.args) = str_to_non_neg(argv[i]);
			}
		}
		else if (OP("rm")) {
			cmd.action = REMOVE;
			if (argv[++i]) {
				cmd.args = malloc(sizeof (size_t));
				*((size_t *) cmd.args) = str_to_non_neg(argv[i]);
			}
		}
		else if (OP("rmlast")) {
			cmd.action = REMOVE_LAST;
			if (argv[++i]) {
				cmd.args = malloc(sizeof (size_t));
				*((size_t *) cmd.args) = str_to_non_neg(argv[i]);
			}
		}
		else if (i == 1)
			cmd.filename = argv[1];
		else break;
		#undef e
	}

	if (cmd.action == INVALID) {
		dump_usage();
		FAIL_IF(1, "Invalid operation\n");
	}

	return cmd;
}
#undef FAIL_IF


static struct blot_LinkedList* read_lines(FILE *input) {
	struct blot_LinkedList *lines = blot_LinkedList_new();

	struct {
		char *chars;
		size_t size;
		ssize_t len;
	} line = { 0 };

	while ((line.len = getline(&line.chars, &line.size, input)) >= 0) {
		if (line.len > 0 && line.chars[line.len-1] == '\n')
			line.chars[line.len-1] = '\0';
		char *copy = malloc(line.size);
		strcpy(copy, line.chars);
		blot_LinkedList_add(lines, copy);
	}
	free(line.chars);

	return lines;
}


int main(int argc, char **argv) {
	// to ensure initialized even when skipped to error handling
	struct blot_LinkedList *lines = NULL;
	FILE *output = NULL;

	errno = 0;
	struct command cmd = parse_command_line(argc, argv);
	if (errno) goto handle_error;

	FILE *input = stdin;
	if (cmd.filename)
		input = fopen(cmd.filename, "r");
	if (input)
		lines = read_lines(input);
	else {
		if (errno != ENOENT) goto handle_error;
		lines = blot_LinkedList_new();
	}


	switch (cmd.action) {
		case PUSH: {
			char **args = cmd.args;
			for (char **arg = args; *arg; ++arg)
				blot_LinkedList_insert(lines, clone(*arg), 0);
		} break;

		case ADD: {
			char **args = cmd.args;
			for (char **arg = args; *arg; ++arg)
				blot_LinkedList_add(lines, clone(*arg));
		} break;

		case INSERT: {
			struct insert_args *args = cmd.args;
			if (args->index > lines->length) goto handle_error;
			blot_LinkedList_insert(lines, clone(args->entry), args->index);
			free(args);
		} break;

		case POP: {
			size_t count = 1;
			if (cmd.args) {
				count = *((size_t *) cmd.args);
				free(cmd.args);
			}
			if (count > lines->length) goto handle_error;
			for (size_t i = 0; i < count; ++i) {
				char *line = blot_LinkedList_remove_at(lines, 0);
				puts(line); free(line);
			}
		} break;

		case REMOVE: {
			if (lines->length == 0) goto handle_error;
			size_t index = lines->length - 1;
			if (cmd.args) {
				index = *((size_t *) cmd.args);
				free(cmd.args);
			}
			if (index >= lines->length) goto handle_error;
			char *line = blot_LinkedList_remove_at(lines, index);
			puts(line); free(line);
		} break;

		case REMOVE_LAST: {
			size_t count = 1;
			if (cmd.args) {
				count = *((size_t *) cmd.args);
				free(cmd.args);
			}
			if (count > lines->length) goto handle_error;
			for (size_t i = 0; i < count; ++i) {
				char *line = blot_LinkedList_remove_at(lines, lines->length-1);
				puts(line); free(line);
			}
		} break;

		case INVALID: goto handle_error;
	}


	output = stdout;
	if (cmd.filename)
		output = fopen(cmd.filename, "w");

	for (struct blot_LinkedListNode *node = lines->head; node; node = node->next) {
    	fputs(node->val, output);
		fputc('\n', output);
	}


	int exit_code = EXIT_SUCCESS;
	goto cleanup;

	handle_error:;
	if (errno) perror(NULL);
	else fputs("Operation failed\n", stderr);
	exit_code = EXIT_FAILURE;

	cleanup:;
	if (output) fclose(output);
	if (lines) {
		for (struct blot_LinkedListNode *node = lines->head; node; node = node->next)
			free(node->val);
		blot_LinkedList_free(lines);
	}

	return exit_code;
}
