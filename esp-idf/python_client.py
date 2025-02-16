import socket

SERVER_IP = "192.168.1.78" 
SERVER_PORT = 5000
file_path = "./test.txt"
def send_file(file_path):
    with open(file_path, "rb") as file:
        data = file.read()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((SERVER_IP, SERVER_PORT))
        client_socket.sendall(data)
        print(f"File {file_path} sent to {SERVER_IP}:{SERVER_PORT}")

# Send a file
send_file("test.txt")
