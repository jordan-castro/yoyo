local println = require("yoyo").println
local fs = require("yoyo.fs")

local contents = fs.read_file("LICENSE")
local entries = fs.read_dir("src")

println("Contents: " .. contents)
println("entries: ")
for i,v in ipairs(entries) do
    println(v)
end