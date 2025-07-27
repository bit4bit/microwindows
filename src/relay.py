import socket
import threading
import logging

# Configure logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

def forward_data(source, destination):
    while True:
        data = source.recv(1024)
        if not data:
            break
        destination.sendall(data)

def handle_connection(client_socket_a, port_b):
    try:
        # Listen on port B
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket_b:
            server_socket_b.bind(('0.0.0.0', port_b))
            server_socket_b.listen(1)
            logging.info(f"Listening for a connection on port {port_b}")

            client_socket_b, addr_b = server_socket_b.accept()
            logging.info(f"Accepted connection from {addr_b} on port {port_b}")

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
            server_socket_a.bind(('0.0.0.0', port_a))
            server_socket_a.listen(1)
            logging.info(f"Listening for a connection on port {port_a}")

            client_socket_a, addr_a = server_socket_a.accept()
            logging.info(f"Accepted connection from {addr_a} on port {port_a}")

            # Handle the connection in a new thread
            threading.Thread(target=handle_connection, args=(client_socket_a, port_b)).start()
    except Exception as e:
        logging.error(f"An error occurred: {e}")

if __name__ == "__main__":
    PORT_A = 8025
    PORT_B = 8029
    main(PORT_A, PORT_B)
