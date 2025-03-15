import socket

SERVER_IP = "192.168.1.74" # Replace with the ESP32's IP address
SERVER_PORT = 5000
filename = "anime.gif"
command = "read"

def request_file(filename):
    file_not_found = True
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((SERVER_IP, SERVER_PORT))

        # 1. Send filename
        client_socket.sendall(filename.encode())

        # 2. Wait for filename confirmation
        response = client_socket.recv(1024).decode()
        if response != "Filename received successfully":
            print(f"Server error: {response}")
            return

        # 3. Send command
        client_socket.sendall(command.encode())

        with open(filename, "wb") as file:
            while True:
                data = client_socket.recv(4096)
                if not data:
                    break
                file.write(data)
                file_not_found = False


        if file_not_found:
            print("File not found")
        else:
            print("File received successfully")

request_file(filename)
