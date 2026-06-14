# YoYo
Strings attached game framework.

Since I first learned Python, I wanted the ability to create a program with multiple different programming langauges. What is known as a 
polygot program. Yoyo, implements polygot programming in a way that ACTUALLY works and does not fight againts you. 
Currently it's being used as the cross platform engine behind [Kazoku]().

## Features

### Scripting
Write in high level scripting languages with native performance. Currently supported languages are
- Lua
- Python
- JavaScript

### Easy Binding
Easily bind C libraries to yoyo programs using the builtin `yoyo.bind` module. 

### Extending
Extend your native program with yoyo by compiling your extension to a shared/static library with `yoyo.make`.

## Installing
TODO

## Building
TODO

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
Here is a more complex program using raylib. We will be using Python for the rest of these examples.

First, yoyo needs to have access to raylib and it's libraries. This can be done easily using `yoyo.bind` module to create the raylib bindings in pixelscript.

To do this we need a build step. For that you will have to write a build.py/lua/js file. An example for raylib would be:
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
from yoyo import cross

# Import the tree. 
# This basically is the same as doing `local Person = require('/path/to/person.lua')` person_lua is a pointer to the Lua Tree in pxs memory.
person_lua = cross.Lua('/path/to/person.lua')
# Call the function.
# This is the same as doing `Person:say_hello()`
person_lua.func('say_hello').call(person_lua.type) 
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

YoYo v0.0.1
>> -- Insert your lua code here!
```

## Compiling for release
Yoyo packages the code without doing any obfuscation at this time. This is a issue and it is currently being worked on.

A yoyo program is compiled with `yoyo compile` at the root of your project. It will create a .exe with the internal files added as raw bytes.

## Package manager
By default, yoyo does not include a package manager. Third Party code can be handled via a `build` script. Include the libraries you want, and apply the build logic for them.

<!-- ## Dependencies
The dependencies required to **use** Yoyo are:
| Name | What it's used for | Recommendations |
|------|--------------------|----------|
| A C/C++ compiler. | Used to compile  -->

<!-- ## Build Dependencies
The dependencies required to **build** Yoyo are:
- Zig -->