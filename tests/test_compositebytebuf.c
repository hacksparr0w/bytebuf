#include <stdbool.h>

#include "bytebuf.h"
#include "unity.h"

void test_initialization(void) {
    compositebytebuf buf;

    compositebytebuf_initialize(&buf);

    TEST_ASSERT_EQUAL(buf.back, NULL);
    TEST_ASSERT_EQUAL(buf.front, NULL);
    TEST_ASSERT_EQUAL(buf.cursor.component, NULL);
    TEST_ASSERT_EQUAL(buf.cursor.position, 0);
    TEST_ASSERT_EQUAL(buf.size, 0);
    TEST_ASSERT_EQUAL(buf.position, 0);
    TEST_ASSERT_TRUE(compositebytebuf_is_empty(&buf));
    TEST_ASSERT_EQUAL(compositebytebuf_remaining(&buf), 0);
}

void test_component_operations(void) {
    compositebytebuf buf;
    compositebytebufreturncode r;
    char *p;

    char data[2][7] = {
        {0x00, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00},
        {0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A}
    };

    size_t sizes[] = {
        4,
        7
    };

    compositebytebuf_initialize(&buf);

    r = compositebytebuf_pop_back(&buf, &p);

    TEST_ASSERT_EQUAL(r, COMPOSITEBYTEBUF_EMPTY_ERROR);

    r = compositebytebuf_pop_front(&buf, &p);

    TEST_ASSERT_EQUAL(r, COMPOSITEBYTEBUF_EMPTY_ERROR);

    r = compositebytebuf_push_back(&buf, sizes[0], data[0]);

    TEST_ASSERT_EQUAL(r, COMPOSITEBYTEBUF_OK);
    TEST_ASSERT_NOT_EQUAL(buf.back, NULL);
    TEST_ASSERT_EQUAL(buf.back->previous, NULL);
    TEST_ASSERT_EQUAL(buf.back->next, NULL);
    TEST_ASSERT_EQUAL(buf.back->size, sizes[0]);
    TEST_ASSERT_EQUAL(buf.back->data, data[0]);
    TEST_ASSERT_EQUAL(buf.front, buf.back);
    TEST_ASSERT_EQUAL(buf.cursor.component, buf.back);
    TEST_ASSERT_EQUAL(buf.cursor.position, 0);
    TEST_ASSERT_EQUAL(buf.size, sizes[0]);
    TEST_ASSERT_EQUAL(buf.position, 0);
    TEST_ASSERT_FALSE(compositebytebuf_is_empty(&buf));
    TEST_ASSERT_EQUAL(compositebytebuf_remaining(&buf), sizes[0]);

    r = compositebytebuf_pop_back(&buf, &p);

    TEST_ASSERT_EQUAL(r, COMPOSITEBYTEBUF_OK);
    TEST_ASSERT_EQUAL(p, data[0]);
    TEST_ASSERT_EQUAL(buf.back, NULL);
    TEST_ASSERT_EQUAL(buf.front, NULL);
    TEST_ASSERT_EQUAL(buf.cursor.component, NULL);
    TEST_ASSERT_EQUAL(buf.cursor.position, 0);
    TEST_ASSERT_EQUAL(buf.size, 0);
    TEST_ASSERT_EQUAL(buf.position, 0);
    TEST_ASSERT_TRUE(compositebytebuf_is_empty(&buf));
    TEST_ASSERT_EQUAL(compositebytebuf_remaining(&buf), 0);

    r = compositebytebuf_push_front(&buf, sizes[0], data[0]);

    TEST_ASSERT_EQUAL(r, COMPOSITEBYTEBUF_OK);
    TEST_ASSERT_NOT_EQUAL(buf.back, NULL);
    TEST_ASSERT_EQUAL(buf.back->previous, NULL);
    TEST_ASSERT_EQUAL(buf.back->next, NULL);
    TEST_ASSERT_EQUAL(buf.back->size, sizes[0]);
    TEST_ASSERT_EQUAL(buf.back->data, data[0]);
    TEST_ASSERT_EQUAL(buf.front, buf.back);
    TEST_ASSERT_EQUAL(buf.cursor.component, buf.back);
    TEST_ASSERT_EQUAL(buf.cursor.position, 0);
    TEST_ASSERT_EQUAL(buf.size, sizes[0]);
    TEST_ASSERT_EQUAL(buf.position, 0);
    TEST_ASSERT_FALSE(compositebytebuf_is_empty(&buf));
    TEST_ASSERT_EQUAL(compositebytebuf_remaining(&buf), sizes[0]);

    r = compositebytebuf_pop_front(&buf, &p);

    TEST_ASSERT_EQUAL(r, COMPOSITEBYTEBUF_OK);
    TEST_ASSERT_EQUAL(p, data[0]);
    TEST_ASSERT_EQUAL(buf.back, NULL);
    TEST_ASSERT_EQUAL(buf.front, NULL);
    TEST_ASSERT_EQUAL(buf.cursor.component, NULL);
    TEST_ASSERT_EQUAL(buf.cursor.position, 0);
    TEST_ASSERT_EQUAL(buf.size, 0);
    TEST_ASSERT_EQUAL(buf.position, 0);
    TEST_ASSERT_TRUE(compositebytebuf_is_empty(&buf));
    TEST_ASSERT_EQUAL(compositebytebuf_remaining(&buf), 0);

    r = compositebytebuf_push_back(&buf, sizes[0], data[0]);

    TEST_ASSERT_EQUAL(r, COMPOSITEBYTEBUF_OK);

    r = compositebytebuf_push_back(&buf, sizes[1], data[1]);
    
    TEST_ASSERT_EQUAL(r, COMPOSITEBYTEBUF_OK);
    TEST_ASSERT_NOT_EQUAL(buf.back, NULL);
    TEST_ASSERT_EQUAL(buf.back->previous, buf.front);
    TEST_ASSERT_EQUAL(buf.back->next, NULL);
    TEST_ASSERT_EQUAL(buf.back->size, sizes[1]);
    TEST_ASSERT_EQUAL(buf.back->data, data[1]);
    TEST_ASSERT_NOT_EQUAL(buf.front, NULL);
    TEST_ASSERT_EQUAL(buf.front->previous, NULL);
    TEST_ASSERT_EQUAL(buf.front->next, buf.back);
    TEST_ASSERT_EQUAL(buf.front->size, sizes[0]);
    TEST_ASSERT_EQUAL(buf.front->data, data[0]);
    TEST_ASSERT_EQUAL(buf.cursor.component, buf.front);
    TEST_ASSERT_EQUAL(buf.cursor.position, 0);
    TEST_ASSERT_EQUAL(buf.size, sizes[0] + sizes[1]);
    TEST_ASSERT_EQUAL(buf.position, 0);
    TEST_ASSERT_FALSE(compositebytebuf_is_empty(&buf));
    TEST_ASSERT_EQUAL(
        compositebytebuf_remaining(&buf),
        sizes[0] + sizes[1]
    );

    r = compositebytebuf_pop_back(&buf, &p);

    TEST_ASSERT_EQUAL(r, COMPOSITEBYTEBUF_OK);
    TEST_ASSERT_NOT_EQUAL(buf.back, NULL);
    TEST_ASSERT_EQUAL(buf.back->previous, NULL);
    TEST_ASSERT_EQUAL(buf.back->next, NULL);
    TEST_ASSERT_EQUAL(buf.back->size, sizes[0]);
    TEST_ASSERT_EQUAL(buf.back->data, data[0]);
    TEST_ASSERT_EQUAL(buf.front, buf.back);
    TEST_ASSERT_EQUAL(buf.cursor.component, buf.back);
    TEST_ASSERT_EQUAL(buf.cursor.position, 0);
    TEST_ASSERT_EQUAL(buf.size, sizes[0]);
    TEST_ASSERT_EQUAL(buf.position, 0);
    TEST_ASSERT_FALSE(compositebytebuf_is_empty(&buf));
    TEST_ASSERT_EQUAL(compositebytebuf_remaining(&buf), sizes[0]);

    r = compositebytebuf_push_front(&buf, sizes[1], data[1]);

    TEST_ASSERT_EQUAL(r, COMPOSITEBYTEBUF_OK);
    TEST_ASSERT_NOT_EQUAL(buf.back, NULL);
    TEST_ASSERT_EQUAL(buf.back->previous, buf.front);
    TEST_ASSERT_EQUAL(buf.back->next, NULL);
    TEST_ASSERT_EQUAL(buf.back->size, sizes[0]);
    TEST_ASSERT_EQUAL(buf.back->data, data[0]);
    TEST_ASSERT_NOT_EQUAL(buf.front, NULL);
    TEST_ASSERT_EQUAL(buf.front->previous, NULL);
    TEST_ASSERT_EQUAL(buf.front->next, buf.back);
    TEST_ASSERT_EQUAL(buf.front->size, sizes[1]);
    TEST_ASSERT_EQUAL(buf.front->data, data[1]);
    TEST_ASSERT_EQUAL(buf.cursor.component, buf.back);
    TEST_ASSERT_EQUAL(buf.cursor.position, 0);
    TEST_ASSERT_EQUAL(buf.size, sizes[0] + sizes[1]);
    TEST_ASSERT_EQUAL(buf.position, sizes[1]);
    TEST_ASSERT_FALSE(compositebytebuf_is_empty(&buf));
    TEST_ASSERT_EQUAL(
        compositebytebuf_remaining(&buf),
        sizes[0]
    );
}
