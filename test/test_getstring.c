/* Test for psh_getstring
 * do `gcc -Wall -Wextra -I. -g -fsanitize=address
 * libpsh/test_getstring.c libpsh/util.c libpsh/xmalloc.c`
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libpsh/util.h"
#include "libpsh/xmalloc.h"

const char *helper(char *buf, size_t len)
{
    const char *data =
        "przbegjjrlvwmqpzcxcozdjbolffxvbgymbbjwzyvrbngwdmnyntaiwwjlipqrajvdewwb"
        "nruilaqdecoohkxdwbznoajwpfywitceeyvyhviyufxhqlcgjzfgbzbdbulinjguedyplz"
        "shgbalkannmamsuevacqsuaqocssgsrqztazwofitqrfcsafvzyocazynewqcwnybnqlfr"
        "rlknadsgxrolwvbpkwojhsqcfmllpsexujzjtutlnzmlnwlgfibqofbshqzrwpvyhalupu"
        "tcfupjwoprjypmaweelujngzppkhjdjwrtfkukgcdsndefaripwnjlyolxoecfqgljzlez"
        "iarfdpjmdezjtrqqcfuwpojpxumkynhfpvkjtmiqycymcptevmwarctwtzsgaaukgntxtz"
        "uwvchfxtrzmjzitkzhzhsniyiicrhdceesgxuuzzwevurtcomvpldxtiohqwzapucsdizv"
        "qjvugveickochzbetrooukjqkuyqonihjvaaenomgyrzzwosfnonzilpdxsqgtksxpzwms"
        "swvpqratgpcjlyzgqscyvkcgdvzpmhlakwgmcvwvvznosaqaejbaqbtymrbdpzduikhncd"
        "tgqrovmvxxwabzhlospsrcxqkzlzyjkfmhetbskuebozrcioraqrwcjfhsnztrvwrkxols"
        "nfgqhwtfiowwgafjsbcgpgyyrzrwiaqydpmqgmyzhvbqigncgvpkpxpkoxqyarimvivzwc"
        "pwoshqwayemousajmwzffrxvdhyqdlwjopyriokhpqblstvfvequfdmmurnglstjfgyrvw"
        "zfwotxearujnllmwpzphsmhifherqkcpexlgjfruxsvdgwmulcreekuhqkunqxzasvacnc"
        "gbeppiytukwfrqzgcoktvptgveufbonqdqzryjwuxphgrycjlvigvlswecnmwcmegwqsft"
        "hbllwbxqdvaphlttkhebauhfflmrgpgrftsfibgaubvs"; /* random characters */
    if (len > 1025)
        len = 1025; /* 1 for \0 */
    memmove(buf, data, len);
    buf[len - 1] = 0;
    return data;
}

int main(void)
{
    char *rest, *ret;
    rest = psh_getstring((void *(*)(char *, size_t)) & helper, (void **)&ret);
    printf("%d\n%s\n%s", strcmp(rest, ret), rest, ret);
    free(rest);
    return 0;
}
