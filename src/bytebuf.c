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

bool compositebytebuf_is_empty(compositebytebuf *buf) {
	return buf->back == NULL;
}

size_t compositebytebuf_remaining(compositebytebuf *buf) {
	return buf->size - buf->position;
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

		buf->back = back->previous;
		buf->back->next = NULL;
		buf->size -= back->size;

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

		buf->front = front->next;
		buf->front->previous = NULL;
		buf->size -= front->size;

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

	return COMPOSITEBYTEBUF_OK;
}

compositebytebufreturncode compositebytebuf_check_bounds(
	compositebytebuf *buf,
	ptrdiff_t delta
) {
	if (delta == 0) {
		return COMPOSITEBYTEBUF_OK;
	} else if (delta > 0) {
		ptrdiff_t remaining = compositebytebuf_remaining(buf);

		if (delta > remaining) {
			return COMPOSITEBYTEBUF_OUT_OF_BOUNDS_ERROR;
		}
	} else {
		ptrdiff_t remaining = buf->position;

		if (-delta > remaining) {
			return COMPOSITEBYTEBUF_OUT_OF_BOUNDS_ERROR;
		}
	}

	return COMPOSITEBYTEBUF_OK;
}

compositebytebufreturncode compositebytebuf_move_position(
	compositebytebuf *buf,
	ptrdiff_t delta,
	bool check_bounds
) {
	if (check_bounds) {
		compositebytebufreturncode result = compositebytebuf_check_bounds(
			buf,
			delta
		);

		if (result != COMPOSITEBYTEBUF_OK) {
			return result;
		}
	}

	buf->position += delta;

	while (delta != 0) {
		if (delta > 0) {
			ptrdiff_t remaining = (
				buf->cursor.component->size - buf->cursor.position
			);

			if (remaining >= delta) {
				buf->cursor.position += delta;
				break;
			} else {
				delta -= remaining;
				buf->cursor.component = buf->cursor.component->next;
				buf->cursor.position = 0;
			}
		} else {
			ptrdiff_t remaining = buf->cursor.position;

			if (remaining >= -delta) {
				buf->cursor.position += delta;
				break;
			} else {
				delta += remaining;
				buf->cursor.component = buf->cursor.component->previous;
				buf->cursor.position = buf->cursor.component->size;
			}
		}
	}

	return COMPOSITEBYTEBUF_OK;
}

compositebytebufreturncode compositebytebuf_set_position(
	compositebytebuf *buf,
	size_t position,
	bool check_bounds
) {
	ptrdiff_t delta = position - buf->position;

	return compositebytebuf_move_position(buf, delta, check_bounds);
}
