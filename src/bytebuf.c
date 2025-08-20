#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bytebuf.h"

#define DEFINE_BYTEBUF_READ_INT(NAME, TYPE, DECODE)                          \
bytebuf_error NAME(bytebuf *buf, TYPE *value) {                              \
    bytebuf_error error;                                                     \
    bytebuf_view view;                                                       \
                                                                             \
    error = bytebuf_read(buf, sizeof(TYPE), &view);                          \
                                                                             \
    if (error) {                                                             \
        return error;                                                        \
    }                                                                        \
                                                                             \
    char *data = view.data;                                                  \
                                                                             \
    *value = DECODE(data);                                                   \
                                                                             \
    if (view.is_copy) {                                                      \
        free(data);                                                          \
    }                                                                        \
                                                                             \
    return BYTEBUF_OK;                                                       \
}

static inline uint8_t decode_uint8(const char *data) {
    return (uint8_t) data[0];
}

static inline uint16_t decode_uint16_be(const char *data) {
    return (
		((uint16_t) (uint8_t) data[0] << 8) |
		(uint8_t) data[1]
	);
}

static inline uint16_t decode_uint16_le(const char *data) {
    return (
		(uint8_t) data[0] |
		((uint16_t) (uint8_t) data[1] << 8)
	);
}

static inline uint32_t decode_uint32_be(const char *data) {
    return (
		((uint32_t) (uint8_t) data[0] << 24) |
        ((uint32_t) (uint8_t) data[1] << 16) |
        ((uint32_t) (uint8_t) data[2] << 8)  |
        (uint8_t) data[3]
	);
}

static inline uint32_t decode_uint32_le(const char *data) {
    return (
		(uint8_t) data[0] |
        ((uint32_t) (uint8_t) data[1] << 8)  |
        ((uint32_t) (uint8_t) data[2] << 16) |
        ((uint32_t) (uint8_t) data[3] << 24)
	);
}

static inline uint64_t decode_uint64_be(const char *data) {
    return (
		((uint64_t) (uint8_t) data[0] << 56) |
        ((uint64_t) (uint8_t) data[1] << 48) |
        ((uint64_t) (uint8_t) data[2] << 40) |
        ((uint64_t) (uint8_t) data[3] << 32) |
        ((uint64_t) (uint8_t) data[4] << 24) |
        ((uint64_t) (uint8_t) data[5] << 16) |
        ((uint64_t) (uint8_t) data[6] << 8)  |
        (uint8_t) data[7]
	);
}

static inline uint64_t decode_uint64_le(const char *data) {
    return (
		(uint8_t) data[0] |
		((uint64_t) (uint8_t) data[1] << 8)  |
		((uint64_t) (uint8_t) data[2] << 16) |
		((uint64_t) (uint8_t) data[3] << 24) |
		((uint64_t) (uint8_t) data[4] << 32) |
		((uint64_t) (uint8_t) data[5] << 40) |
		((uint64_t) (uint8_t) data[6] << 48) |
		((uint64_t) (uint8_t) data[7] << 56)
	);
}

static inline int8_t decode_int8(const char *data) {
    return (int8_t) decode_uint8(data);
}

static inline int16_t decode_int16_be(const char *data) {
    return (int16_t) decode_uint16_be(data);
}

static inline int16_t decode_int16_le(const char *data) {
    return (int16_t) decode_uint16_le(data);
}

static inline int32_t decode_int32_be(const char *data) {
    return (int32_t) decode_uint32_be(data);
}

static inline int32_t decode_int32_le(const char *data) {
    return (int32_t) decode_uint32_le(data);
}

static inline int64_t decode_int64_be(const char *data) {
    return (int64_t) decode_uint64_be(data);
}

static inline int64_t decode_int64_le(const char *data) {
    return (int64_t) decode_uint64_le(data);
}

static inline bytebuf_error bytebuf_check_bounds(
	bytebuf *buf,
	ptrdiff_t delta
) {
	if (delta == 0) {
		return BYTEBUF_OK;
	} else if (delta > 0) {
		ptrdiff_t remaining = bytebuf_remaining(buf);

		if (delta > remaining) {
			return BYTEBUF_BOUNDS_ERROR;
		}
	} else {
		ptrdiff_t remaining = buf->position;

		if (-delta > remaining) {
			return BYTEBUF_BOUNDS_ERROR;
		}
	}

	return BYTEBUF_OK;
}

void bytebuf_initialize(bytebuf *buf) {
	buf->back = NULL;
	buf->front = NULL;
	buf->cursor.component = NULL;
	buf->cursor.position = 0;

	buf->position = 0;
	buf->size = 0;
}

bool bytebuf_is_empty(bytebuf *buf) {
	return buf->back == NULL;
}

size_t bytebuf_remaining(bytebuf *buf) {
	return buf->size - buf->position;
}

bytebuf_error bytebuf_pop_back(bytebuf *buf, char **data) {
	bytebuf_component *back = buf->back;

	if (back == NULL) {
		return BYTEBUF_EMPTY_ERROR;
	}

	if (back == buf->front) {
		*data = back->data;

		bytebuf_initialize(buf);

		free(back);

		return BYTEBUF_OK;
	}

	*data = back->data;

	if (buf->cursor.component == back) {
		buf->position -= buf->cursor.position;
		buf->cursor.component = back->previous;
		buf->cursor.position = back->previous->size;
	}

	buf->back = back->previous;
	buf->back->next = NULL;
	buf->size -= back->size;

	free(back);

	return BYTEBUF_OK;
}

bytebuf_error bytebuf_pop_front(bytebuf *buf, char **data) {
	bytebuf_component *front = buf->front;

	if (front == NULL) {
		return BYTEBUF_EMPTY_ERROR;
	}

	if (front == buf->back) {
		*data = front->data;

		bytebuf_initialize(buf);

		free(front);

		return BYTEBUF_OK;
	}

	*data = front->data;

	if (buf->cursor.component == front) {
		buf->position = 0;
		buf->cursor.component = front->next;
		buf->cursor.position = 0;
	}

	buf->front = front->next;
	buf->front->previous = NULL;
	buf->size -= front->size;

	free(front);

	return BYTEBUF_OK;
}

bytebuf_error bytebuf_push_back(bytebuf *buf, size_t size, char *data) {
	bytebuf_component *component = malloc(sizeof(*component));

	if (component == NULL) {
		return BYTEBUF_ALLOCATOR_ERROR;
	}

	component->size = size;
	component->data = data;
	component->previous = NULL;
	component->next = NULL;

	if (buf->back == NULL) {
		buf->back = component;
		buf->front = component;
		buf->cursor.component = component;
	} else {
		component->previous = buf->back;
		buf->back->next = component;
		buf->back = component;
	}

	buf->size += size;

	return BYTEBUF_OK;
}

bytebuf_error bytebuf_push_front(bytebuf *buf, size_t size, char *data) {
	bytebuf_component *component = malloc(sizeof(*component));

	if (component == NULL) {
		return BYTEBUF_ALLOCATOR_ERROR;
	}

	component->size = size;
	component->data = data;
	component->previous = NULL;
	component->next = NULL;

	if (buf->front == NULL) {
		buf->front = component;
		buf->back = component;
		buf->cursor.component = component;
	} else {
		component->next = buf->front;
		buf->front->previous = component;
		buf->front = component;
		buf->position += size;
	}

	buf->size += size;

	return BYTEBUF_OK;
}

bytebuf_error bytebuf_move_position(bytebuf *buf, ptrdiff_t delta) {
	bytebuf_error error = bytebuf_check_bounds(buf, delta);

	if (error) {
		return error;
	}

	ptrdiff_t i = delta;

	while (i != 0) {
		if (i > 0) {
			ptrdiff_t remaining = (
				buf->cursor.component->size - buf->cursor.position
			);

			if (remaining >= i) {
				buf->cursor.position += i;

				break;
			} else {
				i -= remaining;
				buf->cursor.component = buf->cursor.component->next;
				buf->cursor.position = 0;
			}
		} else {
			ptrdiff_t remaining = buf->cursor.position;

			if (remaining >= -i) {
				buf->cursor.position += i;

				break;
			} else {
				i += remaining;
				buf->cursor.component = buf->cursor.component->previous;
				buf->cursor.position = buf->cursor.component->size;
			}
		}
	}

	buf->position += delta;

	return BYTEBUF_OK;
}

bytebuf_error bytebuf_set_position(bytebuf *buf, size_t position) {
	return bytebuf_move_position(buf, position - buf->position);
}

bytebuf_error bytebuf_read(bytebuf *buf, size_t size, bytebuf_view *view) {
	bytebuf_error error = bytebuf_check_bounds(buf, size);

	if (error) {
		return error;
	}

	size_t i = size;
	size_t copied = 0;
	char *data = NULL;

	while (i != 0) {
		size_t remaining = (
			buf->cursor.component->size - buf->cursor.position
		);

		if (remaining >= i) {
			if (data == NULL) {
				data = buf->cursor.component->data + buf->cursor.position;
			} else {
				memcpy(
					data + copied,
					buf->cursor.component->data + buf->cursor.position,
					i
				);
			}

			buf->cursor.position += i;

			break;
		} else if (remaining == 0) {
			buf->cursor.component = buf->cursor.component->next;
			buf->cursor.position = 0;
		} else {
			if (data == NULL) {
				data = malloc(size);

				if (data == NULL) {
					return BYTEBUF_ALLOCATOR_ERROR;
				}
			}

			memcpy(
				data + copied,
				buf->cursor.component->data + buf->cursor.position,
				remaining
			);

			copied += remaining;
			i -= remaining;
			buf->cursor.component = buf->cursor.component->next;
			buf->cursor.position = 0;
		}
	}

	buf->position += size;
	view->size = size;
	view->data = data;
	view->is_copy = copied > 0;

	return BYTEBUF_OK;
}

DEFINE_BYTEBUF_READ_INT(bytebuf_read_int8, int8_t, decode_int8)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_int16_be, int16_t, decode_int16_be)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_int16_le, int16_t, decode_int16_le)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_int32_be, int32_t, decode_int32_be)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_int32_le, int32_t, decode_int32_le)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_int64_be, int64_t, decode_int64_be)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_int64_le, int64_t, decode_int64_le)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_uint8, uint8_t, decode_uint8)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_uint16_be, uint16_t, decode_uint16_be)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_uint16_le, uint16_t, decode_uint16_le)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_uint32_be, uint32_t, decode_uint32_be)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_uint32_le, uint32_t, decode_uint32_le)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_uint64_be, uint64_t, decode_uint64_be)
DEFINE_BYTEBUF_READ_INT(bytebuf_read_uint64_le, uint64_t, decode_uint64_le)
