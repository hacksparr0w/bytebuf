#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef enum bytebufreturncode {
	BYTEBUF_OK = 0,
	BYTEBUF_MALLOC_ERROR = 1,
	BYTEBUF_EMPTY_ERROR = 2,
} bytebufreturncode;

typedef struct bytebufcomponent {
	size_t size;
	char *data;

	struct bytebufcomponent *previous;
	struct bytebufcomponent *next;
} bytebufcomponent;

typedef struct bytebufreadresult {
	size_t size;
	char *data;

	bool is_copy;
} bytebufreadresult;

typedef struct compositebytebuf {
	bytebufcomponent *front;
	bytebufcomponent *back;
	bytebufcomponent *cursor;

	size_t position;
	size_t size;
	size_t remaining;
} compositebytebuf;

inline void compositebytebuf_initialize(compositebytebuf *buf) {
	buf->front = NULL;
	buf->back = NULL;
	buf->cursor = NULL;

	buf->position = 0;
	buf->size = 0;
	buf->remaining = 0;
}

bytebufreturncode compositebytebuf_pop_back(
	compositebytebuf *buf,
	char **data
);

bytebufreturncode compositebytebuf_pop_front(
	compositebytebuf *buf,
	char **data
);

bytebufreturncode compositebytebuf_push_back(
	compositebytebuf *buf,
	size_t size,
	const char *data
);

bytebufreturncode compositebytebuf_push_front(
	compositebytebuf *buf,
	size_t size,
	const char *data
);
