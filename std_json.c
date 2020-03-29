/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE W` ARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "std_json.h"
#include <stdbool.h>

#define STD_LOCAL_LOG_LEVEL STD_LOG_INFO
void json_free(void *ptr, int count)
{
    if(count < 0)
    {
        return;
    }

    if(count == 0)
    {
        STD_FREE(ptr);
        return;
    }
        
    char **ptr_list = (char **)ptr;
    for(int n = 0; n < count; n++)
    {
        STD_FREE(ptr_list[n]);
    }
    STD_FREE(ptr);
}

int __json_parse_object(const char *json_str, const char *key, void *value, int value_type)
{
    cJSON *pJson = cJSON_Parse(json_str);
    STD_CHECK_RETURN(pJson != NULL);
    cJSON *pSub = NULL;
    if(key != NULL)
        pSub = cJSON_GetObjectItem(pJson, key);
    else
        pSub = pJson;

    STD_CHECK_GOTO_FAIL(pSub != NULL);
    char *pSub_raw     = NULL;
    int array_size     = 0;

    switch (value_type) {
        case 1:
            *((char *)value) = pSub->valueint;
            break;

        case 2:
            *((short *)value) = pSub->valueint;
            break;

        case 3:
            *((int *)value) = pSub->valueint;
            break;

        case 4:
            *((float *)value) = (float)(pSub->valuedouble);
            break;

        case 5:
            *((double *)value) = (double)pSub->valuedouble;
            break;

        default:
            switch (pSub->type) {
                case cJSON_False:
                    *((char *)value) = false;
                    break;

                case cJSON_True:
                    *((char *)value) = true;
                    break;

                case cJSON_Number:
                    *((char *)value) = pSub->valueint;
                    break;

                case cJSON_NULL:
                    //do nothing, so default value must be null
                    break;

                case cJSON_String:
                    *((char **)value) = STD_MALLOC(strlen(pSub->valuestring) + 1);
                    memcpy(*((char **)value), pSub->valuestring, strlen(pSub->valuestring) + 1);
                    break;

                case cJSON_Object:
                    pSub_raw = cJSON_PrintUnformatted(pSub);
                    STD_CHECK_GOTO_FAIL(pSub_raw != NULL);
                    *((char **)value) = pSub_raw;
                    break;

                case cJSON_Array:
                    array_size = cJSON_GetArraySize(pSub);
                    if(array_size <= 0)
                    {
                        cJSON_Delete(pJson);
                        return 0;
                    }
                    char **index = STD_MALLOC(array_size * sizeof(char *));
                    *((char ***)value) = index;
                    for (int i = 0; i < array_size; i++)
                    {
                        cJSON *item = cJSON_GetArrayItem(pSub, i);
                        
                        if (item->type == cJSON_Number) {
                            *((int *)value + i) = item->valueint;
                            continue;
                        }

                        if (item->type == cJSON_String) {
                            int len = strlen(item->valuestring);
                            index[i] = STD_MALLOC(len + 1);
                            memcpy(index[i] , item->valuestring, len + 1);
                            continue;
                        }

                        if (item->type == cJSON_Object) {
                            char *temp = cJSON_PrintUnformatted(item);
                            STD_CHECK_GOTO_FAIL(temp != NULL);
                            *index = temp;
                            index ++;
                        }

                        /**< no sub cJSON_Array, just support one layer of cJSON_Array */
                    }

                    /**< cJSON_Array parsed successful, return array_size */
                    cJSON_Delete(pJson);
                    return array_size;
                    break;

                default:
                    STD_LOGE("does not support this type(%d) of data parsing\r\n", pSub->type);
                    break;
            }
    }

    cJSON_Delete(pJson);
    return 0;

FAIL:
    cJSON_Delete(pJson);
    STD_LOGE("key[%s] parse fail", key);
    return -1;
}

int json_parse_object_raw(const char *json_str, const char *key, char **object_str)
{
    cJSON *pJson = cJSON_Parse(json_str);
    STD_CHECK_RETURN(pJson != NULL);
    cJSON *pSub = NULL;
    pSub = cJSON_GetObjectItem(pJson, key);

    if (!pSub) {
        STD_LOGE("cJSON_GetObjectItem, key: %s\r\n", key);
        goto FAIL;
    }

    char *pSub_raw     = NULL; 
    pSub_raw = cJSON_PrintUnformatted(pSub);
    STD_CHECK_GOTO_FAIL(pSub_raw != NULL);
    *object_str = pSub_raw;

    cJSON_Delete(pJson);
    return 0;   
FAIL:
    cJSON_Delete(pJson);
    return -1;
}


ssize_t __json_pack_object(char *json_str, const char *key, void *value, int value_type)
{
    STD_ASSERT(json_str != NULL);
    STD_ASSERT(key != NULL);

    STD_LOGV("value_type: %d\r\n", value_type);

    /**< start symbol of a json object */
    char identifier = '{';

    /**< pack data into array that has been existed */
    if (*key == '[') {
        identifier = '[';
        key = NULL;
    }

    int index = 0;

    if (*json_str == identifier) {
        index     = strlen(json_str) - 1;
        json_str += index;
        *json_str = ',';
    } else {
        *json_str = identifier;
    }

    /**< forward to key field by "++" */
    json_str++;
    index++;

    /**< pack key into json_str */
    int tmp_len = key ? sprintf(json_str, "\"%s\":", key) : 0;
    json_str += tmp_len;
    index    += tmp_len;
    char *string = *((char **)value);
    switch (value_type) {
        case 1: /**/
            tmp_len = sprintf(json_str, "%d", *((bool *)value));
            break;

        case 2: /**/
            tmp_len = sprintf(json_str, "%d", *((int8_t *)value));
            break;

        case 3: /**/
            tmp_len = sprintf(json_str, "%d", *((uint8_t *)value));
            break;

        case 4: /**/
            tmp_len = sprintf(json_str, "%d", *((short *)value));
            break;

        case 5: /**/
            tmp_len = sprintf(json_str, "%d", *((uint16_t *)value));
            break;

        case 6: /**/
            tmp_len = sprintf(json_str, "%d", *((int *)value));
            break;

        case 7: /**/
            tmp_len = sprintf(json_str, "%d", *((uint32_t *)value));
            break;

        case 8: /**/
            tmp_len = sprintf(json_str, "%ld", *((long *)value));
            break;

        case 9: /**/
            tmp_len = sprintf(json_str, "%ld", *((unsigned long *)value));
            break;

        case 10: /**/
            tmp_len = sprintf(json_str, "%f", *((float *)value));
            break;

        case 11: /**/
            tmp_len = sprintf(json_str, "%lf", *((double *)value));
            break;

        case 12: 
            if (*string == '{' || *string == '[') {
                tmp_len = sprintf(json_str, "%s", (char *)string);
            } else {
                tmp_len = sprintf(json_str, "\"%s\"", (char *)string);
            }
            break;

        default:
            STD_LOGE("key[%s] invalid type[%d]\r\n", key, value_type);
            STD_ASSERT(-1);
            return -1;
    }

    /**< update json_str and index after packed key */
    json_str += tmp_len;
    index    += tmp_len;

    /**< finish json_str with '}' or ']' */
    *json_str = identifier == '{' ? '}' : ']';
    json_str++;
    *json_str = '\0';

    index++;

    return index;
}
