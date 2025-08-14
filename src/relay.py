import socket
import threading
import logging
import sys

# Configure logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

def forward_data(source, destination):
    while True:
        data = source.recv(8192)
        src_ip, src_port = source.getpeername()
        dst_ip, dst_port = destination.getpeername()
        hex_data = ' '.join(f'{byte:02x}' for byte in data)
        logging.debug(f"Data from {src_ip}:{src_port} to {dst_ip}:{dst_port} - Hexdump: {hex_data}")
        if not data:
            sys.exit()
            break
        destination.sendall(data)

def handle_connection(client_socket_a, port_b):
    try:
        # Listen on port B
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket_b:
            server_socket_b.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server_socket_b.bind(('0.0.0.0', port_b))
            server_socket_b.listen(1)
            logging.info(f"Listening for a connection on port {port_b}")

            client_socket_b, addr_b = server_socket_b.accept()
            logging.info(f"Accepted connection from {addr_b} on port {port_b}")
            socket_opts(client_socket_b)

            client_socket_a.sendall(b"READY")
            # Forward data between port A and B
            threading.Thread(target=forward_data, args=(client_socket_b, client_socket_a)).start()
            threading.Thread(target=forward_data, args=(client_socket_a, client_socket_b)).start()
    except Exception as e:
        logging.error(f"An error occurred: {e}")

def main(port_a, port_b):
    try:
        # Listen on port A
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket_a:
            server_socket_a.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server_socket_a.bind(('0.0.0.0', port_a))
            server_socket_a.listen(1)
            logging.info(f"Listening for a connection on port {port_a}")

            client_socket_a, addr_a = server_socket_a.accept()
            socket_opts(client_socket_a)
            logging.info(f"Accepted connection from {addr_a} on port {port_a}")

            # Handle the connection in a new thread
            threading.Thread(target=handle_connection, args=(client_socket_a, port_b)).start()
    except Exception as e:
        logging.error(f"An error occurred: {e}")

def socket_opts(sck):
    sck.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    sck.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1)
    sck.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 1)
if __name__ == "__main__":
    PORT_A = 8026
    PORT_B = 6600
    main(PORT_A, PORT_B)
