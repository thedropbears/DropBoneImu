from socket import socket, AF_INET, SOCK_DGRAM

def main():
    sock = socket(AF_INET, SOCK_DGRAM)
    sock.bind(('', 4774)) # bind to all interfaces/addresses by default
    while True:
        packet = sock.recv(1024)
        exploded = [float(val) for val in packet.split(',')]
        print exploded

if __name__ == '__main__':
    main()
