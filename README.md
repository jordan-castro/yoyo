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
|YOYO_OS|`yoyo.os`|This is cross platform.|
|YOYO_PXS|`yoyo.pxs`|Adds methods for working with `pixelscript`.|
|YOYO_FS|`yoyo.fs`|Adds file system access. This is cross platform.|
|YOYO_SHELL|`yoyo.shell`|Interact with system shell. This is cross platform.|
|YOYO_NET|`yoyo.net`|Adds low and high level networking/http. Uses `reqwests` in rust.|
|YOYO_ZIP|`yoyo.zip`|Read/Write/Extract zip files.|

NOTE:
Although some modules are cross platform that does not mean they will 100% work on something like a Samsung fridge. But if you need
to build for that... you probably are not using YoYo anyways...


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
<!-- ### compiling scripting code
```bash
yoyo compile test.lua
```
There is a caveat that this basic `compile` command will only work if you are not linking external libraries.
Otherwise it is better for you to create a `build` script and run it with `yoyo build.py` or lua,js.

But as this is still a WIP, I have not created the `yoyo.build` module. -->

## Pixelscript
Yoyo proudly uses [pixelscript](https://github.com/jordan-castro/pixelscript)

## Structure
This project is outlined as follows

|path|intention|
|----|---------|
|`core/`|Core yoyo modules as raw C/C++.|
|`pixelscript/`|Pixelscript submodule used for the boostrap builder.|
|`test/`|Tests cases. Run via `yoyo test`|
|`src/`| Yoyo Cli source code.|
|`scripts/`|Useful scripts for development. Require `yoyo` bootstrap atleast.|
|`extensions/`|Extensions required for building `yoyo`.|

## Build instructions
