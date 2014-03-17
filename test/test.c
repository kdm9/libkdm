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
 *       Compiler:  gcc
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
    km_free(dat, &tst_err_handler);
    tt_ptr_op(dat, ==, NULL);
end:
    ;
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
