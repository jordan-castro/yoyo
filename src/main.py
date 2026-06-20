# from yoyo import os
from yoyo import *
from yoyo import os
from src.commands.repl import repl
from src.commands.run import run
from src.arg_parser import ArgParser, Flag

VERSION = "0.0.3"

USAGE = f"""yoyo v{VERSION}

-h, --help              Get this message

repl <language>         Start a pixelscript repl.
<file>                  Run a specific file.
"""

def main():
    argp = ArgParser(os.argv)
    argp.parse()
    # skip yoyo
    argp.next_arg()

    # Get command
    command = argp.next_arg()

    if not command:
        print(USAGE)
        return

    try:
        match command:
            case 'repl':
                repl(argp)
            case _:
                # Try and run a file
                run(argp)
    except Exception as e:
        println(str(e))


if __name__ == "__main__":
    main()