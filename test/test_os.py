from yoyo import println
from yoyo import os


println("Os argv:", os.argv)
cwd = os.get_cwd()
println("Current dir:", cwd)
os.ch_dir('pixelscript')
println("Current dir:", os.get_cwd())
os.ch_dir(cwd)