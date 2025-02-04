#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP ""  // ESP32's IP, run the server first to get it and then replace the value here, then rerun
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024


/*Flushes input buffer to prevent freezing*/
void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { } // Flush input buffer
}


/*Function for Uploading files to ESP32 server*/
void upload_file(int sock) {
    char filename[50];
    char buffer[1024];
    FILE *file;
    int len;

    printf("\nEnter the filename to send: ");
    scanf("%s", filename);

    // Send the UPLOAD command
    write(sock, "UPLOAD", strlen("UPLOAD"));

    // Send the filename
    write(sock, filename, strlen(filename));

    // Wait for server acknowledgment
    len = read(sock, buffer, sizeof(buffer) - 1);
    buffer[len] = '\0';

    if (strcmp(buffer, "OK") != 0) {
        printf("\nServer did not acknowledge the filename.\n");
        return;
    }

    // Open the file
    file = fopen(filename, "rb");
    if (!file) {
        printf("\nError: Cannot open file.\n");
        return;
    }

    // Send file data
    while ((len = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(sock, buffer, len);
    }

    fclose(file);
    printf("\nFile '%s' uploaded successfully.\n", filename);
}

/*Function to download file from the ESP32 server*/
void download_file(int sock) {
    char filename[50];
    char buffer[BUFFER_SIZE];
    FILE *file;
    int len;

    printf("\nEnter the filename to download: ");
    scanf("%s", filename);

    // Send command
    write(sock, "DOWNLOAD", strlen("DOWNLOAD"));

    // Send filename
    write(sock, filename, strlen(filename));

    // Receive response
    len = read(sock, buffer, sizeof(buffer) - 1);
    buffer[len] = '\0';

    if (strcmp(buffer, "FILE_NOT_FOUND") == 0) {
        printf("\nError: File '%s' does not exist on the server.\n", filename);
        return;
    }

    // Open file for writing
    file = fopen(filename, "wb");
    if (!file) {
        printf("\nError: Cannot create file.\n");
        return;
    }

    // Receive and save file data
    while ((len = read(sock, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, len, file);
    }

    fclose(file);
    printf("\nFile '%s' downloaded successfully.\n", filename);
}




// Function to list files from the ESP32 server
void list_files(int sock) {
    char buffer[1024];
    int len;

    // Send "LIST" command
    write(sock, "LIST", strlen("LIST"));

    // Receive response
    len = read(sock, buffer, sizeof(buffer) - 1);
    if (len > 0) {
        buffer[len] = '\0';
        printf("\nFiles on ESP32:\n%s", buffer);
    } else {
        printf("\nError: No response from server.\n");
    }
}

/*Function to remove files from the ESP32 server*/
void delete_file(int sock) {
    char filename[50];
    char response[50];
    int len;

    printf("\nEnter the filename to delete: ");
    scanf("%s", filename);

    // Send DELETE command
    write(sock, "DELETE", strlen("DELETE"));

    // Send filename
    write(sock, filename, strlen(filename));

    // Receive server response
    len = read(sock, response, sizeof(response) - 1);
    response[len] = '\0';

    if (strcmp(response, "FILE_NOT_FOUND") == 0) {
        printf("\nError: File '%s' does not exist on the server.\n", filename);
    } else if (strcmp(response, "FILE_DELETED") == 0) {
        printf("\nFile '%s' deleted successfully.\n", filename);
    } else {
        printf("\nError: Could not delete file '%s'.\n", filename);
    }
}


/*Main Application*/
int main() {
    struct sockaddr_in server_addr;
    int sock;
    int choice;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket creation failed.\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Failed to connect to the server.\n");
        return 1;
    }

    printf("Connected to the server.\n");

    while (1) {
        printf("\n1. Upload File");
        printf("\n2. Download File");
        printf("\n3. List Files");
        printf("\n4. Delete File"); 
        printf("\n5. Exit");
        printf("\nChoose: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input. Please enter a number.\n");
            clear_stdin();  // Flush buffer to prevent issues
            continue;
        }

        clear_stdin(); // Make sure input buffer is clean

        if (choice == 1) {
            upload_file(sock);
        } 
        else if (choice == 2) {
            download_file(sock);
        } 
        else if (choice == 3) {
            list_files(sock);
        } 
        else if (choice == 4) {
            delete_file(sock);
        } 
        else if (choice == 5) {
            write(sock, "EXIT", strlen("EXIT"));
            close(sock);
            printf("\nDisconnected from the server.\n");
            break;
        } 
        else {
            printf("\nInvalid choice. Try again.\n");
        }
    }   
    
    return 0;
}
