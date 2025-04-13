import socket

# Create a raw socket and bind it to all interfaces
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.settimeout(15)
s.bind(("127.0.1.202", 5025))
s.listen()
con, addr = s.accept()
print("Accepted :", addr)

while True:
    bytes = con.recv(256)
    string = str(bytes, encoding='ascii')
    print(f'Received packet from {addr} >> {string}')
    con.send(b"Hello! from server. but I'm not tea pot\n")
