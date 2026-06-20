# YoYo
A engine for a dreamer.

I dont really know how to describe this yet. But basically it lets you compile python, lua, and JS into native binaries.
It also lets you define your own ecosystem. `yoyo` comes with a builtin ecosystem that you can 
enable or disable as you please.

The flags are
|name|purpose|notes|
|----|-------|-----|
|YOYO_DEBUG|Adds debugging to the `yoyo` cli.|Not really useful outside of `yoyo`s development.|
|YOYO_CORE|Adds the main `yoyo` module.|Adds `print`, `println`, `readln`. If you dont add YOYO_CORE you can still add submodules.|
|YOYO_OS|Adds the `yoyo.os` module.|This is platform agnostic but may not work on embedded.|
|YOYO_PXS|Adds the `yoyo.pxs` module.|Adds methods for working with `pixelscript`.|
|YOYO_FS|Adds the `yoyo.fs` module.|Adds file system access. This is platform agnostic but may not work on embedded.|


## How to use
### repl
You can use it as a repl.
```bash
yoyo repl py # or lua, js
> yoyo.println("Hello World!")
Hello World!
```
### running a file
```bash
yoyo test.lua
Test from lua
```
### compiling pixelscript on the fly
```bash
yoyo pxs
```
### compiling scripting code
```bash
yoyo compile test.lua
```
There is a caveat that this basic `compile` command will only work if you are not linking external libraries.
Otherwise it is better for you to create a `build` script and run it with `yoyo build.py` or lua,js.

But as this is still a WIP, I have not created the `yoyo.build` module.

## Pixelscript
Yoyo proudly uses [pixelscript](https://github.com/jordan-castro/pixelscript)