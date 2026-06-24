from yoyo import pxs
from yoyo import println


def run_test(runtime, language, code):
    println(f"Test {language} {runtime} {language} {code}")
    res = pxs.exec(runtime, code, f"<{language}>")
    if not res == None:
        raise BaseException(res)

run_test(pxs.PYTHON, "python", "1 + 1")
run_test(pxs.LUA, "lua", "local res = 1 + 1")
run_test(pxs.JS, "js", "1 + 1;")

