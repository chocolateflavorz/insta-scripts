from PyPDF2 import PdfFileMerger
import sys
import glob

arg_length = len(sys.argv)
arg_list = sys.argv[1:arg_length]

merger = PdfFileMerger()

if arg_list[0] == 'all':
    arg_list = glob.glob('*.pdf', recursive = True)

for pdf in arg_list:
    merger.append(pdf)

merger.write("result.pdf")
merger.close()
