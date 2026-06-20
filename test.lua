local println = require("yoyo").println
local os = require("yoyo.os")

println(os.get_cwd())
os.ch_dir("pixelscript")
println(os.get_cwd())