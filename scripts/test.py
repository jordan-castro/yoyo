from yoyo import fs
from yoyo import shell
from yoyo import pxs
from yoyo import *


# read test dir
files = fs.read_dir("test")
failed = []
for f in files:
    path = f"test/{f}"
    println(f"============= TEST {path} =============")
    contents = fs.read_file(path)
    if len(contents) == 0:
        println("Contents are empty")
        failed.append(path)
        continue

    file_runtime = pxs.runtime_from_name(f.split('.')[-1])
    if file_runtime == None:
        failed.append(path)
        continue
    result = pxs.exec(file_runtime, contents, path)
    if result != None:
        println(result)
        failed.append(path)

println("================= = = = = = =====================")

if len(failed) == 0:
    println("All tests passed")
else:
    println("Tests that failed: " + ",".join(failed))