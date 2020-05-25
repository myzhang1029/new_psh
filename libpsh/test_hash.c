/* Test for psh_hash
 * do `gcc -DNO_READLINE -Wall -Wextra -I. -g -fsanitize=address
 * libpsh/test_hash.c libpsh/hash.c libpsh/hasher.c libpsh/util.c
 * libpsh/xmalloc.c`
 */

#include <string.h>

#include "libpsh/hash.h"
#include "libpsh/xmalloc.h"

#define add_hash(a, b, c, d) add_hash_chk(&a, b, c, d)

int main(void)
{
    char *val1;
    int *val2;
    psh_hash *hash;

    val1 = xmalloc(P_CS * 9);
    strcpy(val1, "abcdefgh");
    val2 = xmalloc(sizeof(int));
    *val2 = 42;

    hash = new_hash(1);
    add_hash(hash, "1", val1, 1);
    printf("%p\n", hash);
    add_hash(hash, "2", val1, 0);
    printf("%p\n", hash);
    add_hash(hash, "2", val2, 0);
    printf("%p\n", hash);
    add_hash(hash, "3", val2, 1);
    printf("%p\n", hash);
    add_hash(hash, "hash", "12345", 0);
    printf("%p\n", hash);
    add_hash(hash, "psh", "12345", 0);
    printf("%p\n", hash);
    add_hash(hash, "sh", "12345", 0);
    printf("%p\n", hash);
    add_hash(hash, "test", "12345", 0);
    printf("%p\n", hash);
    add_hash(hash, "dill", "12345", 0);
    printf("%p\n", hash);

    puts(get_hash(hash, "1"));                   /* abcdefgh */
    printf("%d\n", *(int *)get_hash(hash, "2")); /* 42 */

    rm_hash(hash, "1");

    printf("%p\n", get_hash(hash, "1")); /* 0x0 */

    free_hash(hash, 1);

    return 0;
}
