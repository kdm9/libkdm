/*
 * ============================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  Test libkdm
 *
 *        Version:  1.0
 *        Created:  05/03/14 21:44:03
 *       Revision:  none
 *        License:  GPLv3+
 *       Compiler:  gcc / clang
 *
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#ifndef _WIN32
#include <unistd.h>
#endif

/* TinyTest */
#include "tinytest.h"
#include "tinytest_macros.h"
/* libkdm */
#include "kdm.h"
#include "testdata.h"

/* Constants */
#define NUM_ZEROS 1<<10
static const unsigned char *zeros[NUM_ZEROS];
static const char *test_data_dir = "." km_pathsep "data";
static const size_t bufsize = 1<<10;

char *
get_test_filename (const char* file)
{
    char * ret = calloc(bufsize, sizeof(*ret));
    int len = snprintf(ret, bufsize, "%s" km_pathsep "%s", test_data_dir, file);
    if (len > 0 && len < bufsize) return ret;
    else return NULL;
}

static int km_test_err = 0;
static char *km_test_err_msg;
void
test_err_handler(int err, char *msg,  char *f, int l)
{
    km_test_err = err;
    km_test_err_msg = msg;
    (void) (f);
    (void) (l);
}

void
test_km_calloc(void *ptr)
{
    void *res = NULL;

    res = km_calloc(1, 1, &km_onerr_nil);
    tt_ptr_op(res, !=, NULL);
    tt_int_op(memcmp(res, zeros, 1), ==, 0);
    free(res);

    /* This should fail */
    res = km_calloc(SIZE_MAX, 1, &test_err_handler);
    tt_ptr_op(res, ==, NULL);
    tt_int_op(km_test_err, ==, 1);

end:
    if (res != NULL) free(res);
    ;
}

void
test_km_malloc(void *ptr)
{
    void *res = NULL;

    res = km_malloc(1, &km_onerr_nil);
    tt_ptr_op(res, !=, NULL);
    free(res);

    /* This should fail */
    res = km_malloc(SIZE_MAX, &test_err_handler);
    tt_ptr_op(res, ==, NULL);
    tt_int_op(km_test_err, ==, 1);

end:
    if (res != NULL) free(res);
    ;
}

void
test_km_realloc(void *ptr)
{
    char *res = NULL;
    char *dat = strdup("test");
    /* Test resizing buffer */
    res = km_realloc(dat, 10, &km_onerr_nil);
    tt_ptr_op(res, !=, NULL);
    tt_int_op(memcmp(res, dat, 5), ==, 0);
    free(res);
    /* This should fail */
    res = km_realloc(dat, SIZE_MAX, &test_err_handler);
    tt_ptr_op(res, ==, NULL);
    tt_int_op(km_test_err, ==, 1);
end:
    if (res != NULL) free(res);
    ;
}

void
test_km_free(void *ptr)
{
    char *dat = strdup("test");
    /* Test freeing buffer */
    tt_ptr_op(dat, !=, NULL);
    km_free(dat, &km_onerr_nil);
    tt_ptr_op(dat, ==, NULL);
    /* This free(NULL) should not fail */
    km_free(dat, &test_err_handler);
    tt_ptr_op(dat, ==, NULL);
end:
    ;
}

void
test_kmroundup32 (void *ptr)
{
    int32_t val = 3;
    tt_int_op(kmroundup32(val), ==, 4);
    val++;
    tt_int_op(kmroundup32(val), ==, 8);
    val = 8;
    tt_int_op(kmroundup32(val), ==, 16);
    val = 262143;
    tt_int_op(kmroundup32(val), ==, 262144);
    /* Bored now, lets assume it works until: */
    val = (1<<30) + 1;
    tt_int_op(kmroundup32(val), ==, -2147483648); /* It's signed, remember. Overflows */
    uint32_t uval = (1u<<31) - 1;
    tt_int_op(kmroundup32(uval), ==, 1u<<31);
    uval++;
    tt_int_op(kmroundup32(uval), ==, 0);
end:
    ;
}

void
test_kmroundup64 (void *ptr)
{
    int64_t val = 3;
    tt_int_op(kmroundup64(val), ==, 4);
    val++;
    tt_int_op(kmroundup64(val), ==, 8);
    val = 8;
    tt_int_op(kmroundup64(val), ==, 16);
    val = 262143llu;
    tt_int_op(kmroundup64(val), ==, 262144);
    /* Bored now, lets assume it works until: */
    val = (1llu<<62) + 1;
    tt_int_op(kmroundup64(val), ==, -9223372036854775808llu); /* It's signed, remember. Overflows */
    uint64_t uval = (1llu<<63) - 1;
    tt_int_op(kmroundup64(uval), ==, 1llu<<63);
    uval = 1llu<<62;
    tt_int_op(kmroundup64(uval), ==, 1llu<<63);
    uval = 63llu;
    tt_int_op(kmroundup64(uval) - 2, ==, 62llu);
end:
    ;
}

void
test_km_readline_realloc (void *ptr)
{
    char *lorem_fn = NULL;
    char *buf = NULL;
    char *smallbuf = NULL;
    const size_t smallbuf_len = 1<<4;
    FILE *fp = NULL;
    ssize_t ret = 0;
    size_t line_num;
    char *nulcp = NULL;
    FILE *nulfp = NULL;
    char *tmpcp = NULL;
    size_t tmpsz = 0;
    size_t our_bufsize = bufsize;
    size_t our_smallbuf_len = smallbuf_len;
    km_test_err = 0;
    /* This should always work, so long as you run it from the right dir */
    lorem_fn = get_test_filename("loremipsum.txt");
    if (lorem_fn == NULL)
        tt_abort_msg("Broken test - get_test_filename failed\n");
    if ((fp = fopen(lorem_fn, "r")) == NULL) {
        fprintf(stderr, "Could not open test file '%s' -- %s\n",
                lorem_fn, strerror(errno));
        tt_skip();
    } else {
        buf = calloc(our_bufsize, sizeof(*buf));
        smallbuf = calloc(our_smallbuf_len, sizeof(*smallbuf));
    }
    for (line_num = 0; line_num < n_loremipsum_lines; line_num++) {
        ret = km_readline_realloc(buf, fp, &our_bufsize, &test_err_handler);
        tt_int_op(km_test_err, ==, 0);
        tt_int_op(strncmp(buf, loremipsum_lines[line_num], our_bufsize), ==, 0);
        tt_int_op(strlen(buf), ==, loremipsum_line_lens[line_num]);
        tt_int_op(ret, ==, loremipsum_line_lens[line_num]);
        tt_int_op(our_bufsize, ==, bufsize);
        km_test_err = 0;
    }
    ret = km_readline_realloc(buf, fp, &our_bufsize, &test_err_handler);
    tt_int_op(km_test_err, ==, 0);
    /* check it leaves  \0 in buf */
    tt_int_op(strncmp(buf, "", our_bufsize), ==, 0);
    tt_int_op(strlen(buf), ==, 0);
    tt_int_op(ret, ==, EOF);
    tt_int_op(our_bufsize, ==, bufsize);
    km_test_err = 0;
    /* Naughty tests that try and make it fail */
    rewind(fp);
    /* Null buf */
    ret = km_readline_realloc(nulcp, fp, &our_bufsize, &test_err_handler);
    tt_int_op(km_test_err, ==, 3);
    tt_int_op(ret, ==, -2);
    tt_int_op(our_bufsize, ==, bufsize);
    km_test_err = 0;
    /* Null fp */
    ret = km_readline_realloc(buf, nulfp, &our_bufsize, &test_err_handler);
    tt_int_op(km_test_err, ==, 3);
    tt_int_op(ret, ==, -2);
    tt_int_op(our_bufsize, ==, bufsize);
    km_test_err = 0;
    /* Both buf & fp null */
    ret = km_readline_realloc(nulcp, nulfp, &our_bufsize, &test_err_handler);
    tt_int_op(km_test_err, ==, 3);
    tt_int_op(ret, ==, -2);
    tt_int_op(our_bufsize, ==, bufsize);
    km_test_err = 0;
    /* Test that should require it to resize the buffer */
    rewind(fp);
    ret = km_readline_realloc(smallbuf, fp, &our_smallbuf_len,
            &test_err_handler);
    tt_int_op(km_test_err, ==, 0);
    tt_int_op(ret, ==, loremipsum_line_lens[0]);
    tt_int_op(strlen(smallbuf), ==, loremipsum_line_lens[0]);
    tmpsz = loremipsum_line_lens[0];
    tt_int_op(our_smallbuf_len, ==, kmroundupz(tmpsz));
end:
    if (lorem_fn != NULL) free(lorem_fn);
    if (buf != NULL) free(buf);
    if (smallbuf != NULL) free(smallbuf);
    if (fp != NULL) fclose(fp);
}


/* List of tests. Don't forget to update this.
   Format is:
   { name, fn, flags, testcase_setup_t *ptr, void * for testcase_setup_t }
 */
struct testcase_t tests[] = {
    { "km_calloc", test_km_calloc,},
    { "km_malloc", test_km_malloc,},
    { "km_realloc", test_km_realloc,},
    { "km_free", test_km_free,},
    { "kmroundup32", test_kmroundup32,},
    { "kmroundup64", test_kmroundup64,},
    { "km_readline_realloc", test_km_readline_realloc,},
    END_OF_TESTCASES
};

/*
 * ============================================================================
 * Below this should just be boilerplate
 * ============================================================================
 */
struct testgroup_t kdm_tests[] = {
    {"libkdm/", tests},
    END_OF_GROUPS
};
int
main (int argc, const char *argv[])
{
    bzero(zeros, NUM_ZEROS);
    return tinytest_main(argc, argv, kdm_tests);
} /* ----------  end of function main  ---------- */
