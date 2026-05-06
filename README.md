# YoYo
Strings attached game framework.

Since I first learned Python, I wanted the ability to create a program with multiple different programming langauges. What is known as a 
polygot program. Yoyo, implements polygot programming in a way that ACTUALLY works and does not fight againts you. 
Currently it's being used as the cross platform engine behind [Kazoku]().

Because yoyo is written in Zig, it is blazingly fast and easily interops C libraries.

## Features
By using [pixelscript]() internally, yoyo supports multiple scripting languages. Currently:
- Lua
- Python
- JS

## Example
### Hello World
Here is a VERY BASIC hello world program in Yoyo with Python:
```py
import yoyo
yoyo.print('Hello World!')
```
And the same in lua
```lua
local yoyo = require('yoyo')
yoyo.print('Hello World!')
```
And in JS
```js
import {print} from 'yoyo';
print('Hello World!');
```

### Raylib
Here is a more complex program using raylib. We will be using Python for the rest of the examples.

First, yoyo needs to have access to raylib and it's libraries. This can be done easily using `yoyo bind` command
`yoyo bind path/to/raylib.h` to create the raylib bindings in pixelscript.

You may also need a build step. For that you will have to write a build.py/lua/js file. An example for raylib would be:
```python
from yoyo import make
from yoyo import bind
raylib_bindings = bind.Bindings(
    './path/to/raylib.h', # header path
    "raylib" # pxs module name
)
raylib_bindings.build()

project_mk = make.Make()
# Path to your raylib library
project_mk.lib('./path/to/libraylib.a')
project_mk.arg('arg', 'value')
# Choose compiler
project_mk.assign('gcc')

# Will build the commands already setup.
project_mk.build()
```
Now write some code!
```python
import raylib # Directly import the bindings you just made.

# Initialization
# --------------------------------------------------------------------------------------
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 450

raylib.InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core kora] example - basic window")
raylib.SetTargetFPS(60)               # Set our game to run at 60 frames-per-second
# --------------------------------------------------------------------------------------

# Main game loop
for not raylib.WindowShouldClose(): # detect widnow close button or ESC key
    # UPDATE
    # TODO: Update varaibles here

    # Draw
    raylib.BeginDrawing()
    raylib.ClearBackground(raylib.RAYWHITE)
    raylib.DrawText("Your first window!", 190, 200, 20, raylib.LIGHTGRAY)
    raylib.EndDrawing()

# De-Initialization
# --------------------------------------------------------------------------------------
raylib.CloseWindow()        # Close window and OpenGL context
# --------------------------------------------------------------------------------------
```

### Running
In order to run the raylib program you have to call `yoyo build`. The first build takes the longest.
Then simply run `yoyo run path/to/main/file.py|lua|js` to see your program running in realtime.

## Cross language
Ok... how do we use Lua in Python, vice versa, and JS? Since Yoyo uses pixelscript as it's runtime engine, it is possible to use multiple languages all within the same context.

Example: Calling Lua from Python
```lua
-- Lets say we have a Lua file that exposes a Person tree
local yoyo = require('yoyo')
local Person = {
    name = "",
    age = 0
}
function Person.say_hello(self) 
    yoyo.print('Hello! My name is: ' .. self.name .. ' And I am ' .. tostring(self.age) .. ' Years old!')
end
return Person
```
```python
# Then in Python we can import the lua file using yoyo
from yoyo import import_file

# Import the tree. 
person_lua = import_file('/path/to/person.lua', import_file.LUA)
# This basically is the same as doing `local Person = require('/path/to/person.lua')` person_lua is a pointer to the Lua Tree in pxs memory.
person_lua.call('say_hello', person_lua)
```

## Repl
Yoyo ships with a bultiin repl for debugging scripts and bindings. Just run

`yoyo repl |language|`

to get started.

### Language options
- lua
- python or py
- javascript or js

```bash
yoyo repl lua

YOYO repl start
>> -- Insert your lua code here!
```

## Compiling for release
Yoyo packages the code without doing any obfuscation at this time. This is a issue and it is currently being worked on.

A yoyo program is compiled with `yoyo compile` at the root of your project. It will create a .exe with the internal files added as raw bytes.

## Package manager
By default, yoyo does not include a package manager. Third Party code can be handled via a `build` script. Include the libraries you want, and apply the build logic for them.

## Dependencies
The dependencies required to use Yoyo are:
- Zig
- Rust

Check your setup with `yoyo doctor`.

### Why Zig?
Zig's cross platform compiler is required in order to embed dependency libraries.

- If you are just using yoyo core APIs, then zig is not required.

### Why Rust?
The pixelscript runtime is written in Rust and it's build system uses Rust.

- If you are 