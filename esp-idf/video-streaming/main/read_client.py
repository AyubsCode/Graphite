import socket
import subprocess
import os
import webbrowser
import time

SERVER_IP = "10.0.0.185"  # Replace with the ESP32's IP address
PORT = 5000
video_path = "Test2.mp4"
BUFFER_SIZE = 1024  # Buffer size

def request_file(video_path):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((SERVER_IP, PORT))
        sock.sendall(video_path.encode())  # Send the video path
        with open(video_path, "wb") as file:
            while True:
                data = sock.recv(1024)
                if not data:
                    break
                file.write(data)
        print("File received successfully")


#Play the video stream
def stream():
    # Play video using ffplay(requires to be install before using)
    # process = subprocess.run(["ffplay", "-autoexit", video_path])

    #this one uses VLC which is avaliable on most systems
    process = subprocess.run(["vlc","--play-and-exit",video_path])
 
    # Check if playback was successful before deleting the file
    if process.returncode == 0:  
        os.remove(video_path)
        print(f"{video_path} has been deleted.")
    else:
        print("Playback may have failed, file not deleted.")

#This is for playing it on the IOS
def IOS_stream():
    # Open VLC
    webbrowser.open(f"vlc://{video_path}")

    # Wait for video duration, then delete
    time.sleep(10)  # Adjust based on video length
    os.remove(video_path)
    print(f"{video_path} deleted.")



request_file(video_path)
print("Ready to stream video!\n")
stream()
