/* Test for psh_hash
 * do `gcc -Wall -Wextra -I. -g -fsanitize=address
 * libpsh/test_hash.c libpsh/hash.c libpsh/hasher.c libpsh/util.c
 * libpsh/xmalloc.c`
 */

#include <string.h>

#include "libpsh/hash.h"
#include "libpsh/xmalloc.h"

#define psh_hash_add(a, b, c, d) psh_hash_add_chk(&a, b, c, d)

int main(void)
{
    char *val1;
    int *val2;
    psh_hash *hash;

    val1 = xmalloc(P_CS * 9);
    strcpy(val1, "abcdefgh");
    val2 = xmalloc(sizeof(int));
    *val2 = 42;

    hash = psh_hash_create(1);
    psh_hash_add(hash, "1", val1, 1);
    printf("%p\n", hash);
    psh_hash_add(hash, "2", val1, 0);
    printf("%p\n", hash);
    psh_hash_add(hash, "2", val2, 0);
    printf("%p\n", hash);
    psh_hash_add(hash, "3", val2, 1);
    printf("%p\n", hash);
    psh_hash_add(hash, "hash", "12345", 0);
    printf("%p\n", hash);
    psh_hash_add(hash, "psh", "12345", 0);
    printf("%p\n", hash);
    psh_hash_add(hash, "sh", "12345", 0);
    printf("%p\n", hash);
    psh_hash_add(hash, "test", "12345", 0);
    printf("%p\n", hash);
    psh_hash_add(hash, "dill", "12345", 0);
    printf("%p\n", hash);

    puts(psh_hash_get(hash, "1"));                   /* abcdefgh */
    printf("%d\n", *(int *)psh_hash_get(hash, "2")); /* 42 */

    psh_hash_rm(hash, "1");

    printf("%p\n", psh_hash_get(hash, "1")); /* 0x0 */

    psh_hash_free(hash, 1);

    return 0;
}
