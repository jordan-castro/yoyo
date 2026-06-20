from yoyo import pxs
from yoyo import *
from src.arg_parser import ArgParser


def _setup_runtime(runtime: int):
    code = ""
    match runtime:
        case pxs.LUA:
            code = "yoyo = require('yoyo')"
        case pxs.PYTHON:
            code = "import yoyo"
        case pxs.JS:
            code = "import * as yoyo from 'yoyo'; globalThis.yoyo = yoyo;"
        case _:
            raise Exception("Runtime not supported")
        
    # run the code
    pxs.exec(runtime, code, "<repl>")


def repl(arg_parser: ArgParser):
    # Get language
    language = (arg_parser.next_arg() or "").lower()
    runtime = -1
    if language == "py" or language == "python":
        runtime = pxs.PYTHON
    elif language == "lua":
        runtime = pxs.LUA
    elif language == "js" or language == "javascript":
        runtime = pxs.JS
    
    if runtime == -1:
        raise Exception("Runtime not supported")

    _setup_runtime(runtime)

    while True:
        print("> ")
        line:str = readln()

        if line.lower() == "quit":
            return

        if len(line) == 0:
            continue

        res = pxs.exec(runtime, line, "<repl>")
        if res:
            println(res)