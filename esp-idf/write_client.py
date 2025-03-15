import socket

SERVER_IP = "192.168.1.70"
SERVER_PORT = 5000
file_path = "../Files/jpeg_43-2.jpeg"
filename = "jpeg_43-2.jpeg"

def send_file(file_path):
    with open(file_path, "rb") as file:
        data = file.read()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((SERVER_IP, SERVER_PORT))
        client_socket.sendall(filename.encode())
        response = client_socket.recv(1024)
        if response.decode() == "Filename received successfully":
            print("ESP32 is ready to receive the file.")
            client_socket.sendall(data)
            print(f"File {file_path} sent to {SERVER_IP}:{SERVER_PORT}")

# Send a file
send_file(file_path)