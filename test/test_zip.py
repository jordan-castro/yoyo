from yoyo import println
from yoyo import zip
from yoyo import fs


# Create a new zip file
file = zip.create()
file.write('file1.txt', 'Ayo my main man dog!')
file.write('file2.txt', 'Dude whatsup!')
file.write('directory/file3.txt', 'Yo a file in a dir dog!')
file.write('dir2/file4.txt', 'More text!')

file.save('test.zip')

rfile = zip.open('test.zip')
entries = rfile.list_dir()
if len(entries) != 4:
    raise Exception("Length not match")

# Extract it
file.extract('file1.txt', 'file1.txt')
contents = fs.read_file('file1.txt')
if not contents == 'Ayo my main man dog!':
    raise Exception('Contents dont match')
fs.remove_file('file1.txt')

# Remove it at the end.
fs.remove_file('test.zip')