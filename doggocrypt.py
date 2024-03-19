import sys
import random

opt = "e"
target_file = "alien.dat"
doggo_file = "doggo"
dict_file = "doggo.dict"

def trim(a):
    r = []
    for e in a:
        if e != '':
            r.append(e)
    return r

def encrypt():
    doggo_list = [i for i in range(256)]
    random.shuffle(doggo_list)
    with open(target_file, 'rb') as f:
        with open(doggo_file, 'wb') as doggo:
            for b in f.read():
                doggo.write(bytes([doggo_list[b]]))
    with open(dict_file, 'wb') as dict:
            dict.write(bytes(doggo_list))
    quit()

def decrypt():
    doggo_list = []
    with open(dict_file, 'rb') as dict:
        for b in dict.read():
            doggo_list.append(b)
    with open(doggo_file, 'rb') as doggo:
        with open(target_file, 'wb') as f:
            for b in doggo.read():
                f.write(bytes([doggo_list.index(b)]))
    quit()

arg = sys.argv[1:len(sys.argv)]
argc = len(arg)

if argc < 2:
    print("doggocrypt.py [e/d] [input]")
    print(" e [target], encrypt [target] for `doggo`, dictionary file for `doggo.dict`")
    print(" d [target], decrypt doggo with dictionary file `doggo.dict` and save to [target] ")
    quit()
else:
    opt = arg[0]
    target_file = arg[1]

if opt == "e":
    encrypt()
elif opt == "d":
    decrypt()
else:
    print("doggocrypt.py [e/d] [input]")
    quit()

