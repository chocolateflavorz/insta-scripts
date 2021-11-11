from PIL import Image, ExifTags
import sys
import os
import glob

arg_length = len(sys.argv)
arg_list = sys.argv[1:arg_length]

if arg_list[0] == 'all':
    arg_list = glob.glob('*.jpg', recursive = True)

for image_path in arg_list:
    image = Image.open(image_path)
    width, height = image.size
    if width > height:
        image = image.rotate(270, expand=True)
    image = image.convert('RGB')
    image.save('downsized_' + image_path)
    image.close()
    print("converted " + image_path + " to " + 'downsized_' + image_path)
