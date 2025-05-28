#include "utils.h"

unsigned char* concat_ordered(const unsigned char* arr1, size_t len1, const unsigned char* arr2, size_t len2, size_t total_len) {
    unsigned char* result = new unsigned char[total_len];
    memcpy(result, arr1, len1);
    memcpy(result + len1, arr2, len2);
    return result;
} 