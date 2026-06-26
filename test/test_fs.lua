local yoyo = require("yoyo")
local fs = require("yoyo.fs")

local println = yoyo.println
local print = yoyo.print

local contents = fs.read_file("LICENSE")
local bytes = fs.read_fileb("CHANGELOG.md")
local entries = fs.read_dir("src")

println("Contents: " .. contents)
println("entries: ")
for i,v in ipairs(entries) do
    println(v)
end
println("bytes: ")
for i, v in ipairs(bytes) do
    print(v, ",")
end
println()

-- Create a new file
fs.write_file('text.txt', 'This is a test!')
-- Read its contents
contents = fs.read_file('text.txt')
if contents ~= "This is a test!" then
    error('file was not written')
end
-- Remove the text.txt file
if fs.remove_file('text.txt') ~= true then
    error('file was not removed')
end
if contents ~= 'This is a test!' then
    error('Contents not equal')
end

-- Write to a directory 
if fs.write_file('dude/perfect.txt', 'T') ~= true then
    error('Could not write file in directory.')
end

contents = fs.read_file('dude/perfect.txt')
if contents ~= 'T' then
    error('File was not written in diretory')
end

fs.remove_dir('dude')