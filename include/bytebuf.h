#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef enum compositebytebufreturncode {
	COMPOSITEBYTEBUF_OK = 0,
	COMPOSITEBYTEBUF_MALLOC_ERROR = 1,
	COMPOSITEBYTEBUF_EMPTY_ERROR = 2
} compositebytebufreturncode;

typedef struct compositebytebufcomponent {
	size_t size;
	char *data;

	struct compositebytebufcomponent *previous;
	struct compositebytebufcomponent *next;
} compositebytebufcomponent;

typedef struct compositebytebufcursor {
	size_t position;
	compositebytebufcomponent *component;
} compositebytebufcursor;

typedef struct compositebytebufreadresult {
	size_t size;
	char *data;

	bool is_copy;
} compositebytebufreadresult;

typedef struct compositebytebuf {
	compositebytebufcomponent *back;
	compositebytebufcomponent *front;
	compositebytebufcursor cursor;

	size_t size;
	size_t position;
} compositebytebuf;

void compositebytebuf_initialize(compositebytebuf *buf);

bool compositebytebuf_is_empty(compositebytebuf *buf);

compositebytebufreturncode compositebytebuf_pop_back(
	compositebytebuf *buf,
	char **data
);

compositebytebufreturncode compositebytebuf_pop_front(
	compositebytebuf *buf,
	char **data
);

compositebytebufreturncode compositebytebuf_push_back(
	compositebytebuf *buf,
	size_t size,
	char *data
);

compositebytebufreturncode compositebytebuf_push_front(
	compositebytebuf *buf,
	size_t size,
	char *data
);

size_t compositebytebuf_remaining(compositebytebuf *buf);
