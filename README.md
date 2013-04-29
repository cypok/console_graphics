# Library for working with console

This library can be used to create application with console UI. It supports:

* colors
* text positioning
* keyboard events handling

Implementation uses:

* `conio.h` and `wincon.h` on Windows
* `ncurses.h` on Unix-based OS

### Using

Just copy `console.h` and `console.c` to your project and use it.

On Unix-based OS you should have
[ncurses](http://www.gnu.org/software/ncurses/ncurses.html) library installed.

### Example

There is an `example.c` which can be useful to understand using of this
library.

![screenshot](https://github.com/cypok/console_graphics/raw/master/screenshot.png)

### Feedback

This library was inspired by Alexey Nikitin and developed together
with Ivan Novikov aka [NIA](https://github.com/NIA).

Any feedback is welcome! Open pull request or email to
vladimir.parfinenko@gmail.com.

Copyright © 2013 Vladimir Parfinenko, released under the MIT license.

