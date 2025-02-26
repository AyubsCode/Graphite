import socket

SERVER_IP = "192.168.1.70" # Replace with the ESP32's IP address
SERVER_PORT = 5000
filename = "jpeg_43-2.jpeg"

def request_file(filename):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((SERVER_IP, SERVER_PORT))
        sock.sendall(filename.encode())  # Send the filename
        with open(filename, "wb") as file:
            while True:
                data = sock.recv(1024)
                if not data:
                    break
                file.write(data)
        print("File received successfully")

# Example usage
request_file(filename)
