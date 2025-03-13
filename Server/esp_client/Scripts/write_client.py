import socket

SERVER_IP = "192.168.1.74"
SERVER_PORT = 5000
file_path = "../Files/anime.gif"
filename = "anime.gif"
command = "write"

def send_file(file_path):
    with open(file_path, "rb") as file:
        data = file.read()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((SERVER_IP, SERVER_PORT))
        client_socket.sendall(filename.encode()) # Send filename
        response = client_socket.recv(1024).decode().strip()
        if response == "Filename received successfully":
            client_socket.sendall(command.encode()) # Send write command
            response = client_socket.recv(1024).decode().strip()
            print("ESP32 is ready to receive the file.")
            if response == "OK":
                client_socket.sendall(data)
        print(f"File {file_path} sent to {SERVER_IP}:{SERVER_PORT}")

# Send a file
send_file(file_path)
