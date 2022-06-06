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
# rotate
rot = 270

arg_length = len(sys.argv)
arg_list = sys.argv[1:arg_length]
img_list = []


for i, arg in enumerate(arg_list):
    if arg == "all":
        target = glob.glob("*.jpg", recursive=True)
        continue
    if arg == "rot":
        rot = 0
        continue
    target = arg_list[i:arg_length]
    break

for f in target:
    img = Image.open(f)
    img = img.rotate(rot, expand=True)
    img = img.resize(SIZE_HDPI)
    img_list.append(img)

img_list[0].save(OUTPUT, save_all=True, append_images=img_list[1:], resolution=300)
