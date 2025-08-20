#include <stdbool.h>
#include <stdint.h>

#include "bytebuf.h"
#include "unity.h"

void test_integral_reading(void) {
    char data[4][4] = {
        {(char) 0xc5, (char) 0x93, (char) 0xd7, (char) 0xc8},
        {(char) 0x4f, (char) 0x19, (char) 0xae, (char) 0xb6}
    };

    bytebuf buf;

    bytebuf_initialize(&buf);

    for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        bytebuf_push_back(&buf, sizeof(data[i]), data[i]);
    }

    int8_t v1;
    int16_t v2;
    int32_t v3;
    int64_t v4;

    bytebuf_read_int8(&buf, &v1);

    TEST_ASSERT_EQUAL(v1, (int8_t) 0xc5);

    bytebuf_set_position(&buf, 0);

    bytebuf_read_int16_be(&buf, &v2);

    TEST_ASSERT_EQUAL(v2, (int16_t) 0xc593);

    bytebuf_set_position(&buf, 0);

    bytebuf_read_int32_be(&buf, &v3);

    TEST_ASSERT_EQUAL(v3, (int32_t) 0xc593d7c8);

    bytebuf_set_position(&buf, 0);

    bytebuf_read_int64_be(&buf, &v4);

    TEST_ASSERT_EQUAL(v4, (int64_t) 0xc593d7c84f19aeb6);
}
