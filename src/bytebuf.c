#include <stdlib.h>

#include "bytebuf.h"

void compositebytebuf_initialize(compositebytebuf *buf) {
	buf->back = NULL;
	buf->front = NULL;
	buf->cursor.component = NULL;
	buf->cursor.position = 0;

	buf->position = 0;
	buf->size = 0;
}

compositebytebufreturncode compositebytebuf_pop_back(
	compositebytebuf *buf,
	char **data
) {
	compositebytebufcomponent *back = buf->back;

	if (back == NULL) {
		return COMPOSITEBYTEBUF_EMPTY_ERROR;
	} else if (back == buf->front) {
		*data = back->data;

		compositebytebuf_initialize(buf);

		free(back);

		return COMPOSITEBYTEBUF_OK;
	} else {
		*data = back->data;

		if (buf->cursor.component == back) {
			buf->position -= buf->cursor.position;
			buf->cursor.component = back->previous;
			buf->cursor.position = back->previous->size;
		}

		buf->size -= back->size;
		buf->back = back->previous;

		free(back);

		return COMPOSITEBYTEBUF_OK;
	}
}

compositebytebufreturncode compositebytebuf_pop_front(
	compositebytebuf *buf,
	char **data
) {
	compositebytebufcomponent *front = buf->front;

	if (front == NULL) {
		return COMPOSITEBYTEBUF_EMPTY_ERROR;
	} else if (front == buf->back) {
		*data = front->data;

		compositebytebuf_initialize(buf);

		free(front);

		return COMPOSITEBYTEBUF_OK;
	} else {
		*data = front->data;

		if (buf->cursor.component == front) {
			buf->position = 0;
			buf->cursor.component = front->next;
			buf->cursor.position = 0;
		}

		buf->size -= front->size;
		buf->front = front->next;

		free(front);

		return COMPOSITEBYTEBUF_OK;
	}
}

compositebytebufreturncode compositebytebuf_push_back(
	compositebytebuf *buf,
	size_t size,
	char *data
) {
	compositebytebufcomponent *component = malloc(
		sizeof(compositebytebufcomponent)
	);

	if (component == NULL) {
		return COMPOSITEBYTEBUF_MALLOC_ERROR;
	}

	component->size = size;
	component->data = data;

	if (buf->back == NULL) {
		buf->back = component;
		buf->front = component;
		buf->cursor.component = component;
	} else {
		component->previous = buf->back;
		buf->back->next = component;
		buf->back = component;
		buf->position -= size;
	}

	buf->size += size;

	return COMPOSITEBYTEBUF_OK;
}

compositebytebufreturncode compositebytebuf_push_front(
	compositebytebuf *buf,
	size_t size,
	char *data
) {
	compositebytebufcomponent *component = malloc(
		sizeof(compositebytebufcomponent)
	);

	if (component == NULL) {
		return COMPOSITEBYTEBUF_MALLOC_ERROR;
	}

	component->size = size;
	component->data = data;

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

	return COMPOSITEBYTEBUF_OK;
}

size_t compositebytebuf_remaining(compositebytebuf *buf) {
	return buf->size - buf->position;
}
