/* Hashing benchmark for psh_hash
 * do `gcc -Wall -Wextra -I. -O3
 * libpsh/bench_hash.c libpsh/hash.c libpsh/hasher.c libpsh/util.c
 * libpsh/xmalloc.c`
 */

#include <stdlib.h>
#include <string.h>

#include "libpsh/hash.h"
#include "libpsh/xmalloc.h"

#define psh_hash_add(a, b, c, d) psh_hash_add_chk(&a, b, c, d)
#define MAX 1000000

int main(void)
{
    char *val1;
    psh_hash *hash;

    val1 = xmalloc(P_CS * 11);
    val1[10] = 0;

    hash = psh_hash_create(16);
    for (int i = 0; i < MAX; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            val1[j] = rand() % 26 + 97;
            psh_hash_add(hash, val1, "a", 0);
        }
    }
    psh_hash_free(hash, 1);
    xfree(val1);

    return 0;
}
