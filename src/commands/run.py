from yoyo import pxs
from yoyo import fs
from src.arg_parser import ArgParser


def run(arg_parser: ArgParser):
    file = arg_parser.current_arg()
    if not file:
        raise Exception("`run` expects a <file>.")

    runtime = -1
    match file.lower().split(".")[-1]:
        case "lua":
            runtime = pxs.LUA
        case "py":
            runtime = pxs.PYTHON
        case "js":
            runtime = pxs.JS

    if runtime == -1:
        raise Exception("file runtime not supported.")

    # open file
    contents = fs.read_file(file)

    res = pxs.exec(runtime, contents, file)

    if res:
        raise Exception(res)
    