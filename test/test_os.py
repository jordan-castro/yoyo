from yoyo import println
from yoyo import os


println("Os argv:", os.argv)
println("Current dir:", os.get_cwd())
os.ch_dir('pixelscript')
println("Current dir:", os.get_cwd())