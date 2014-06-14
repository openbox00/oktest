#include <stdint.h>
#include <string.h>
#include <okl4/env.h>
#include <stdio.h>

const int okl4_api_version = 0x2;

/* Pointer to environment string */
okl4_env_t *__okl4_environ;

char _toupper(const char c);

char
_toupper(const char c)
{
    if (c < 'a' || c > 'z') {
        return c;
    } else {
        return c + 'A' - 'a';
    }
}

void *
okl4_env_get(const char * name)
{
    okl4_word_t i;
    okl4_word_t j;

    for (i = 0; i < __okl4_environ->len; i++) {

        for (j = 0; ; j++) {
            if (_toupper(name[j]) !=
                    _toupper(__okl4_environ->env_item[i].name[j])) {
                break;
            } else if (name[j] == '\0') {
                return (__okl4_environ->env_item[i].item);
            } else {
                continue;
            }
        }
    }
    return NULL;
}


