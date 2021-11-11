from PIL import Image
from fpdf import FPDF
import sys
import os
import glob

SIZE = (210, 297)
OUTPUT = 'imgpdf.pdf'
margin = 0

arg_length = len(sys.argv)
arg_list = sys.argv[1:arg_length]

if arg_list[0] == 'all':
    arg_list = glob.glob('*.jpg', recursive = True)

image = Image.open(arg_list[0])
width, height = image.size
pdf = FPDF(unit="pt", format=[width + 2 * margin, height + 2 * margin])
pdf.add_page()
pdf.image(arg_list[0], margin, margin)
image.close()

arg_list = arg_list[1:]

for image_path in arg_list:
    image = Image.open(image_path)
    pdf.add_page()
    pdf.image(image_path, margin, margin)
    image.close()

pdf.output(OUTPUT, "F")
