# Library for working with console

This library can be used to create application with console UI. It supports:

* colors
* text positioning
* keyboard events handling

Implementation uses:

* `conio.h` and `wincon.h` in Windows
* `ncurses.h` in Unix-based

### Using

Just copy `console.h` and `src/console_{win or unix}.c` to your project and use
it.

### Example

There is an `example/example.c` which can be useful to understand using of this
library.

### Feedback

This library was inspired by Alexey Nikitin and developed together
with Ivan Novikov aka [NIA](https://github.com/NIA).

Any feedback is welcome! Open pull request or email to
vladimir.parfinenko@gmail.com.

Copyright © 2013 Vladimir Parfinenko, released under the MIT license.

