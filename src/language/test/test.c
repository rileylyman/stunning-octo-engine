#include "unity.h"
#include "language/raw_vector.h"
#include <stdbool.h>

void setUp() {
}

void tearDown() {
}

void test_Raw_Vector_Of_Int() {
    uint32_t x;
    struct RawVector vec = raw_vector_create(sizeof(uint32_t), 4);
    TEST_ASSERT_EQUAL_MESSAGE(raw_vector_size(&vec), 0, "Vector should have 0 elements at first!\n");

    x = 1;
    raw_vector_push_back(&vec, &x);
    TEST_ASSERT_EQUAL_MESSAGE(raw_vector_size(&vec), 1, "Vector should have 1 element after 1 push!\n");
    uint32_t array1[] = { 1 };
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE((uint32_t *)vec.data, array1, 1, "Vector should be [1]\n");
    raw_vector_pop_back(&vec);
    TEST_ASSERT_EQUAL_MESSAGE(raw_vector_size(&vec), 0, "Vector should have 0 element after 1 pop!\n");

    x = 1;
    raw_vector_push_back(&vec, &x);
    x = 2;
    raw_vector_push_back(&vec, &x);
    x = 3;
    raw_vector_push_back(&vec, &x);
    x = 4;
    raw_vector_push_back(&vec, &x);
    x = 5;
    raw_vector_push_back(&vec, &x);
    TEST_ASSERT_EQUAL_MESSAGE(raw_vector_size(&vec), 5, "Vector should have 5 elements after 5 pushes!\n");
    uint32_t array2[] = {1, 2, 3, 4, 5};
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE((uint32_t*) vec.data, array2, 5, "Vector should be [2,2,2,2,2]");

    uint32_t array3[] = {6,7,8,9,10,11,12,13};
    raw_vector_extend_back(&vec, array3, 8);
    TEST_ASSERT_EQUAL_MESSAGE(raw_vector_size(&vec), 13, "Vector should have 13 elements after 5 pushes and 1 extend!\n");
    uint32_t array4[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE((uint32_t*) vec.data, array4, 13, "Vector should be [1,2,3,4,5,6,7,8,9,10,11,12,13]");

    x = 100;
    raw_vector_set(&vec, 4, &x);
    TEST_ASSERT_EQUAL_MESSAGE(raw_vector_size(&vec), 13, "Vector should have same number of elements after first set");
    uint32_t array4_1[] = {1,2,3,4,100,6,7,8,9,10,11,12,13};
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE((uint32_t*) vec.data, array4_1, 13, "Vector should be [1,2,3,4,100,6,7,8,9,10,11,12,13]");

    x = 5;
    raw_vector_set(&vec, 4, &x);
    TEST_ASSERT_EQUAL_MESSAGE(raw_vector_size(&vec), 13, "Vector should have same number of elements after second set");
    uint32_t array4_2[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE((uint32_t*) vec.data, array4_2, 13, "Vector should be [1,2,3,4,5,6,7,8,9,10,11,12,13]");

    for (int i = 0; i < 13; i++) {
        TEST_ASSERT_EQUAL_INT32_MESSAGE(i + 1, *raw_vector_get_ptr(&vec, i), "calling get on each index should yield index + 1");
    }

    raw_vector_clear(&vec);
    TEST_ASSERT_EQUAL_MESSAGE(raw_vector_size(&vec), 0, "Vector should have 0 elements after clearing\n");
    x = 4;
    raw_vector_push_back(&vec, &x);
    x = 5;
    raw_vector_push_back(&vec, &x);
    TEST_ASSERT_EQUAL_MESSAGE(raw_vector_size(&vec), 2, "Vector should have 2 elements after clearing\n");
    uint32_t array5[] = {4,5};
    TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE((uint32_t*) vec.data, array5, 2, "Vector should be [4,5]");

    raw_vector_destroy(&vec);
    TEST_ASSERT_NULL_MESSAGE(vec.data, "Vector data should be nullptr after destorying\n");
}

void test_Raw_Vector_Of_String() {
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_Raw_Vector_Of_Int);
    RUN_TEST(test_Raw_Vector_Of_String);
    return UNITY_END();
}