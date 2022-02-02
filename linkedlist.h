/*
To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with
this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef BLOT_LINKEDLIST_H
#define BLOT_LINKEDLIST_H

#include <stddef.h>
#include <stdbool.h>

struct blot_LinkedListNode {
	void *val;
	struct blot_LinkedListNode *prev, *next;
};

struct blot_LinkedList {
	struct blot_LinkedListNode *head, *tail;
	size_t length;
};

/** Initializes a new LinkedList. */
struct blot_LinkedList * blot_LinkedList_new(void);

/** Adds an element at the end of the list. */
void blot_LinkedList_add(struct blot_LinkedList *, void *);

/** Insert an element at the specified index in the list. */
void blot_LinkedList_insert(struct blot_LinkedList *, void *, size_t);

/**
 * Removes the first element that equals the specified element from the list.
 * If the comparison function is NULL, == is used for comparison.
 * @return true if the list is modified as a result, false otherwise
 */
bool blot_LinkedList_remove(
		struct blot_LinkedList *,
		void *,
		bool (*)(void *, void *));

/** Removes and returns the element at the specified index. */
void * blot_LinkedList_remove_at(struct blot_LinkedList *, size_t);

/**
 * Checks if the LinkedList contains an item using the specified function.
 * If the comparison function is NULL, == is used for comparison.
 */
bool blot_LinkedList_contains(
		struct blot_LinkedList const *,
		void *,
		bool (*)(void *, void *));

/** Frees the specified list. */
void blot_LinkedList_free(struct blot_LinkedList *);

#endif
