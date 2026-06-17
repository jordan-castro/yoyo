# Script that pulls down pixelscript.zip from github and places it inside the yoyo executable.
from urllib.request import urlopen


PIXEL_SCRIPT_ZIP_URL = "https://github.com/jordan-castro/pixelscript/archive/refs/heads/master.zip"
PIXEL_SCRIPT_ZIP_PATH = "pixelscript.zip"

with urlopen(PIXEL_SCRIPT_ZIP_URL) as response:
    data = response.read()
    # Save to pixelscript.zip
    with open(PIXEL_SCRIPT_ZIP_PATH, "wb") as file:
        file.write(data)