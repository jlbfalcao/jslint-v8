
Jslint v8 command
=================

This work was inspired by jsbeautify implementation.

Compile:
    g++ -o jslint jslint.cpp -I<V8_BASEDIR>/include/ -lv8 -L<V8_BASEDIR>/lib -lpthread

Running
=======

    jslint file.js
