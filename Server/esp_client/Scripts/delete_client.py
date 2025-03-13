import socket

SERVER_IP = "192.168.1.74" # Replace with the ESP32's IP address
SERVER_PORT = 5000
filename = "anime.gif"
command = "delete"

def request_file(filename):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((SERVER_IP, SERVER_PORT))
        client_socket.sendall(filename.encode()) # Send filename
        response = client_socket.recv(1024)
        if response.decode() == "Filename received successfully":
            client_socket.sendall(command.encode()) # Send delete command
            response = client_socket.recv(1024)
            if response.decode() == "OK":
                print("File deleted successfully")

request_file(filename)
