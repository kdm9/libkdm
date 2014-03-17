/*
 * ============================================================================
 *
 *       Filename:  testdata.h
 *
 *    Description:  Data for tests of libkdm
 *
 *        Version:  1.0
 *        Created:  17/03/14 16:26:59
 *       Revision:  none
 *        License:  GPLv3+
 *       Compiler:  gcc
 *
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#ifndef TESTDATA_H
#define TESTDATA_H

static const size_t n_loremipsum_lines = 11;
static const size_t loremipsum_line_lens[] = {
    80, 76, 80, 75, 80, 79, 77, 75, 69, 1, 20
};
static const char *loremipsum_lines[] = {
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec ornare tortor et\n",
    "rhoncus iaculis. Sed suscipit, arcu nec elementum vestibulum, tortor tortor\n",
    "dictum dui, eu sodales magna orci eu libero. Cras commodo, ligula tempor auctor\n",
    "vulputate, eros urna gravida eros, eget congue leo quam quis mi. Curabitur\n",
    "luctus augue nibh, eget vehicula augue commodo eget. Donec condimentum molestie\n",
    "adipiscing. In non purus lacus. Nam nec mollis mauris. Donec rhoncus, diam sit\n",
    "amet rhoncus viverra, lectus risus tincidunt ipsum, in dignissim justo purus\n",
    "eget enim. Fusce congue nulla egestas est auctor faucibus. Integer feugiat\n",
    "molestie leo, a interdum neque pretium nec. Etiam sit amet nibh leo.\n",
    "\n",
    "End of lorem ipsum.\n",
};

#endif /* TESTDATA_H */
