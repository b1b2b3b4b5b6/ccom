
#ifndef __STD_JOSN_H__
#define __STD_JOSN_H__

#include "std_common.h"

#ifdef __cplusplus
extern "C" {
#endif /**< _cplusplus */
void json_free(void *ptr, int count);

int json_parse_object_raw(const char *json_str, const char *key, char **object_str);

ssize_t __json_pack_object(char *json_str, const char *key, void *value, int value_type);

#define json_pack_object(json_str, key, value) \
    __json_pack_object((char *)(json_str), ( const char *)(key), (value), \
                    + __builtin_types_compatible_p(typeof(value), bool *) * 1\
                    + __builtin_types_compatible_p(typeof(value), int8_t *) * 2\
                    + __builtin_types_compatible_p(typeof(value), uint8_t *) * 3\
                    + __builtin_types_compatible_p(typeof(value), short *) * 4\
                    + __builtin_types_compatible_p(typeof(value), uint16_t *) * 5\
                    + __builtin_types_compatible_p(typeof(value), int *) * 6\
                    + __builtin_types_compatible_p(typeof(value), uint32_t *) * 7\
                    + __builtin_types_compatible_p(typeof(value), long *) * 8\
                    + __builtin_types_compatible_p(typeof(value), unsigned long *) * 9\
                    + __builtin_types_compatible_p(typeof(value), float *) * 10\
                    + __builtin_types_compatible_p(typeof(value), double *) * 11\
                    + __builtin_types_compatible_p(typeof(value), char **) * 12\
                    + __builtin_types_compatible_p(typeof(value), char *[]) * 12\
                    + __builtin_types_compatible_p(typeof(value), const char *[]) * 12\
                    + __builtin_types_compatible_p(typeof(value), const char **) * 12\
                    + __builtin_types_compatible_p(typeof(value), unsigned char **) * 12\
                    + __builtin_types_compatible_p(typeof(value), const unsigned char **) * 12)

esp_err_t __json_parse_object(const char *json_str, const char *key, void *value, int value_type);
#define json_parse_object(json_str, key, value) \
    __json_parse_object(( const char *)(json_str), ( const char *)(key), (value), \
                     __builtin_types_compatible_p(typeof(value), int8_t *) * 1\
                     + __builtin_types_compatible_p(typeof(value), uint8_t *) * 1\
                     + __builtin_types_compatible_p(typeof(value), bool *) * 1\
                     + __builtin_types_compatible_p(typeof(value), short *) * 2\
                     + __builtin_types_compatible_p(typeof(value), uint16_t *) * 2\
                     + __builtin_types_compatible_p(typeof(value), int *) * 3\
                     + __builtin_types_compatible_p(typeof(value), uint32_t *) * 3\
                     + __builtin_types_compatible_p(typeof(value), long *) * 3\
                     + __builtin_types_compatible_p(typeof(value), unsigned long *) * 3\
                     + __builtin_types_compatible_p(typeof(value), float *) * 4\
                     + __builtin_types_compatible_p(typeof(value), double *) * 5\
                     + __builtin_types_compatible_p(typeof(value), char **) * 6\
                     + __builtin_types_compatible_p(typeof(value), char ***) * 7)\

#ifdef __cplusplus
}
#endif /**< _cplusplus */

#endif 
