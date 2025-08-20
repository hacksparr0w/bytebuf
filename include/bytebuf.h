#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum bytebuf_error {
	BYTEBUF_OK = 0,
	BYTEBUF_ALLOCATOR_ERROR = 1,
	BYTEBUF_BOUNDS_ERROR = 2,
	BYTEBUF_EMPTY_ERROR = 3
} bytebuf_error;

typedef struct bytebuf_component {
	size_t size;
	char *data;

	struct bytebuf_component *next;
	struct bytebuf_component *previous;
} bytebuf_component;

typedef struct bytebuf_cursor {
	size_t position;
	bytebuf_component *component;
} bytebuf_cursor;

typedef struct bytebuf_view {
	size_t size;
	char *data;

	bool is_copy;
} bytebuf_view;

typedef struct bytebuf {
	bytebuf_component *back;
	bytebuf_component *front;
	bytebuf_cursor cursor;

	size_t size;
	size_t position;
} bytebuf;

void bytebuf_initialize(bytebuf *buf);

bool bytebuf_is_empty(bytebuf *buf);

size_t bytebuf_remaining(bytebuf *buf);

bytebuf_error bytebuf_pop_back(bytebuf *buf, char **data);
bytebuf_error bytebuf_pop_front(bytebuf *buf, char **data);
bytebuf_error bytebuf_push_back(bytebuf *buf, size_t size, char *data);
bytebuf_error bytebuf_push_front(bytebuf *buf, size_t size, char *data);

bytebuf_error bytebuf_move_position(bytebuf *buf, ptrdiff_t delta);
bytebuf_error bytebuf_set_position(bytebuf *buf, size_t position);

bytebuf_error bytebuf_read(bytebuf *buf, size_t size, bytebuf_view *view);

bytebuf_error bytebuf_read_int8(bytebuf *buf, int8_t *value);
bytebuf_error bytebuf_read_int16_be(bytebuf *buf, int16_t *value);
bytebuf_error bytebuf_read_int16_le(bytebuf *buf, int16_t *value);
bytebuf_error bytebuf_read_int32_be(bytebuf *buf, int32_t *value);
bytebuf_error bytebuf_read_int32_le(bytebuf *buf, int32_t *value);
bytebuf_error bytebuf_read_int64_be(bytebuf *buf, int64_t *value);
bytebuf_error bytebuf_read_int64_le(bytebuf *buf, int64_t *value);

bytebuf_error bytebuf_read_uint16_be(bytebuf *buf, uint16_t *value);
bytebuf_error bytebuf_read_uint16_le(bytebuf *buf, uint16_t *value);
bytebuf_error bytebuf_read_uint32_be(bytebuf *buf, uint32_t *value);
bytebuf_error bytebuf_read_uint32_le(bytebuf *buf, uint32_t *value);
bytebuf_error bytebuf_read_uint64_be(bytebuf *buf, uint64_t *value);
bytebuf_error bytebuf_read_uint64_le(bytebuf *buf, uint64_t *value);
