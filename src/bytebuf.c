#include "bytebuf.h"

bytebufreturncode compositebytebuf_pop_back(
	compositebytebuf *buf,
	char **data
) {
	bytebufcomponent *back = buf->back;

	if (back == NULL) {
		return BYTEBUF_EMPTY_ERROR;
	} else if (back == buf->front) {
		*data = back->data;

		compositebytebuf_initialize(buf);

		free(back);

		return BYTEBUF_OK;
	} else {
		*data = back->data;

		if (buf->cursor == back) {
			buf->cursor = back->previous;

			buf->position = 0;
			buf->remaining += buf->cursor->size;
		}
		
		buf->size -= back->size;
		buf->remaining -= back->size;
		buf->back = back->previous;

		free(back);

		return BYTEBUF_OK;
	}
}

bytebufreturncode compositebytebuf_pop_front(
	compositebytebuf *buf,
	char **data
) {
	bytebufcomponent *front = buf->front;

	if (front == NULL) {
		return BYTEBUF_EMPTY_ERROR;
	} else if (front == buf->back) {
		*data = front->data;

		compositebytebuf_initialize(buf);

		free(front);

		return BYTEBUF_OK;
	} else {
		*data = front->data;

		if (buf->cursor == front) {
			buf->cursor = front->next;

			buf->remaining -= front->size - buf->position;
			buf->position = 0;
		}

		buf->size -= front->size;
		buf->front = front->next;

		free(front);

		return BYTEBUF_OK;
	}
}

bytebufreturncode compositebytebuf_push_back(
	compositebytebuf *buf,
	size_t size,
	const char *data
) {
	bytebufcomponent *element = malloc(sizeof(bytebufcomponent));

	if (element == NULL) {
		return BYTEBUF_MALLOC_ERROR;
	}

	element->size = size;
	element->data = data;

	if (buf->back == NULL) {
		buf->back = element;
		buf->front = element;
	} else {
		element->previous = buf->back;
		buf->back->next = element;
		buf->back = element;
	}

	buf->size += size;
	buf->remaining += size;

	return BYTEBUF_OK;
}

bytebufreturncode compositebytebuf_push_front(
	compositebytebuf *buf,
	size_t size,
	const char *data
) {
	bytebufcomponent *element = malloc(sizeof(bytebufcomponent));

	if (element == NULL) {
		return BYTEBUF_MALLOC_ERROR;
	}

	element->size = size;
	element->data = data;
	

	if (buf->front == NULL) {
		buf->front = element;
		buf->back = element;
	} else {
		element->next= buf->front;
		buf->front->previous = element;
		buf->front = element;
	}

	buf->size += size;

	return BYTEBUF_OK;
}
