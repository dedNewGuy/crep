# Crep

A command line utility like grep but crappier. It only support basic string matching and does
not support regex. Even though it is crappier, it is easy to use and can be easily use with
Neovim and Emacs since it print logging using `path:line:col:message` format

## Build

Compile it using your favourites C compiler.

## Usage

```
crep <pattern> <file...>
```
