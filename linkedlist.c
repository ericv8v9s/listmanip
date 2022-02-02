/*
To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with
this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#include "linkedlist.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

struct blot_LinkedList * blot_LinkedList_new() {
	struct blot_LinkedList *this = malloc(sizeof (struct blot_LinkedList));
	*this = (struct blot_LinkedList) { 0 };
	return this;
}


void blot_LinkedList_add(struct blot_LinkedList *this, void *val) {
	typedef struct blot_LinkedListNode Node;
	if (this == NULL) return;
	if (this->length == 0) {
		this->head = this->tail = malloc(sizeof (*this->head));
		*this->head = (Node) { val };
	}
	else {
		Node *new_node
				= this->tail->next
				= malloc(sizeof (*new_node));
		*new_node = (Node) { val, this->tail, NULL };
		this->tail = new_node;
	}
	++this->length;
}


void blot_LinkedList_insert(
		struct blot_LinkedList *this,
		void *val,
		size_t index) {
	typedef struct blot_LinkedListNode Node;
	if (this == NULL || index > this->length) return;

	// insert at end is add
	if (index == this->length)
		blot_LinkedList_add(this, val);

	else {	// insert in the middle
		// skip to index
		Node *node = this->head;
		for (size_t i = 0; i < index; ++i, node = node->next);
		// insert new node
		Node *new_node = malloc(sizeof (*new_node));
		*new_node = (Node) { val, node->prev, node };
		if (node == this->head)
			this->head = new_node;
		else
			node->prev->next = new_node;
		node->prev = new_node;
		++this->length;
	}
}


bool blot_LinkedList_remove(
		struct blot_LinkedList *this,
		void *val,
		bool (*equals)(void *, void *)) {
	typedef struct blot_LinkedListNode Node;
	if (this == NULL || this->length == 0) return false;
	if (this->length == 1) {
		if (equals ? (*equals)(this->tail->val, val) : this->tail->val == val) {
			free(this->tail);
			this->head = this->tail = NULL;
			--this->length; return true;
		}
		return false;
	}

	// remove first
	if (equals ? (*equals)(this->head->val, val) : this->head->val == val) {
		Node *removed = this->head;
		this->head = this->head->next;
		this->head->prev = NULL;
		free(removed);
		--this->length; return true;
	}
	// remove last
	else if (equals ? (*equals)(this->tail->val, val) : this->tail->val == val) {
		Node *removed = this->tail;
		this->tail = this->tail->prev;
		this->tail->next = NULL;
		free(removed);
		--this->length; return true;
	}

	// remove middle
	for (Node *node = this->head->next; node != this->tail; node = node->next) {
		if (equals ? (*equals)(node->val, val) : node->val == val) {
			if (node->prev != NULL)
				node->prev->next = node->next;
			if (node->next != NULL)
				node->next->prev = node->prev;
			free(node);
			--this->length; return true;
		}
	}
	return false;
}


void * blot_LinkedList_remove_at(struct blot_LinkedList *this, size_t index) {
	typedef struct blot_LinkedListNode Node;
	if (this == NULL || this->length == 0 || index >= this->length)
		return NULL;
	if (this->length == 1) {
		void *result = this->tail->val;
		free(this->tail);
		this->head = this->tail = NULL;
		--this->length; return result;
	}

	Node *node;
	if (index == 0) {
		node = this->head;
		this->head = this->head->next;
		this->head->prev = NULL;
	}
	else if (index == this->length-1) {
		node = this->tail;
		this->tail = this->tail->prev;
		this->tail->next = NULL;
	}
	else {
		node = this->head->next;
		for (size_t i = 1; i < index; ++i, node = node->next);
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}

	void *result = node->val;
	free(node);
	--this->length; return result;
}


bool blot_LinkedList_contains(
		struct blot_LinkedList const *this,
		void *val,
		bool (*equals)(void *, void *)) {
	typedef struct blot_LinkedListNode Node;
	if (this == NULL || this->length == 0) return false;

	for (Node *node = this->head; node != NULL; node = node->next) {
		if (equals ? (*equals)(node->val, val) : node->val == val)
			return true;
	}
	return false;
}


void blot_LinkedList_free(struct blot_LinkedList *this) {
	if (this == NULL) return;
	for (struct blot_LinkedListNode *node = this->head; node != NULL; ) {
		struct blot_LinkedListNode *next_node = node->next;
		free(node);
		node = next_node;
	}
	free(this);
}
