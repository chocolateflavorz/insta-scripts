# imgpdf ver2

from PIL import Image
import sys
import os
import glob

# A4 72dpi
SIZE = (595, 842)
# a4 300dpi
SIZE_HDPI = (2480, 3508)
# output filename
OUTPUT = "imgpdf.pdf"

arg_length = len(sys.argv)
arg_list = sys.argv[1:arg_length]
img_list = []

if arg_list[0] == "all":
    arg_list = glob.glob("*.jpg", recursive=True)

for arg in arg_list:
    img = Image.open(arg)
    img = img.rotate(270, expand=True)
    img = img.resize(SIZE_HDPI)
    img_list.append(img)

img_list[0].save(OUTPUT, save_all=True, append_images=img_list[1:], resolution=300)
