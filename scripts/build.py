from yoyo import os
from yoyo import process
from yoyo import env
from yoyo import ziplib

os.chdir('pixelscript')
process.call([""])

# import requests
# import shutil
# import os
# import subprocess
# import pathlib


# ZIP_URL = "https://github.com/jordan-castro/pixelscript/archive/refs/heads/master.zip"
# ZIP_LOC = "pixelscript.zip"

# # Download zip
# request = requests.get(ZIP_URL)

# if request.status_code != 200:
#     print('status code: ', request.status_code)
#     exit(1)

# with open(ZIP_LOC, "wb") as file:
#     file.write(request.content)

# request.close()

# # Delete current pixelscript
# shutil.rmtree("pixelscript")

# # Now to unzip!
# shutil.unpack_archive(ZIP_LOC, "./")
# # Rename
# shutil.move("pixelscript-master", "pixelscript")
# # Run the compilation script
# os.chdir("pixelscript")
# subprocess.call(["python", "scripts/build.py", "clear"])
# if not pathlib.Path("pxsb").exists():
#     print('pxsb was not compiled')
#     exit(1)
