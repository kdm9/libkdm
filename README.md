libkdm
======

Header-only C Library which makes life easier

Just `#include "kdm.h"` and you're away.


Tests
-----

To run tests:

    git clone --recursive git@github.com:kdmurray91/libkdm.git # gets testing deps
    cd libkdm
    mkdir build && cd build
    cmake ..
    make && make test
