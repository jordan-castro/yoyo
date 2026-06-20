from yoyo import fs
from yoyo import shell
from yoyo import *


# read test dir
files = fs.read_dir("test")
failed = []
for f in files:
    path = f"test/{f}"
    println("============= TEST =============")
    # TODO: add unix support. I will do that once I build it on my mac for youtube.
    result = shell.system(f".\\build\\Debug\\yoyo.exe {path}")

    if result != 0:
        println(f"{path} failed. Code: {result}")
        files.append(path)

if len(failed) == 0:
    println("All tests passed")
else:
    println("Tests that failed: " + ",".join(failed))