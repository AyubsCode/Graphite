# Graphite

**Graphite** is a capstone project developed for the fourth year of our academic program. It is an innovative solution designed to extend the storage capacity of mobile phones using an external Bluetooth hard drive. This project combines the functionality of an ESP32 development board with a companion mobile app to provide seamless and efficient storage expansion.

## Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [Components](#components)
- [How It Works](#how-it-works)
- [Installation](#installation)
- [Usage](#usage)
- [Technical Details](#technical-details)
- [Acknowledgements](#acknowledgements)
- [Development Document](#document)

## Project Overview

Graphite aims to address the problem of limited storage on mobile devices by offering a wireless solution for storage expansion. By leveraging Bluetooth connectivity and the ESP32 development board, Graphite allows users to easily offload files from their phones to an external hard drive without the need for physical connections.

## Features

- **Bluetooth Connectivity:** Seamlessly connect your mobile device to the external hard drive via Bluetooth.
- **Easy Storage Management:** Use the companion app to manage file transfers between your phone and the external hard drive.
- **Expandable Storage:** Increase your phone’s storage capacity without the need for cables or complex setups.
- **ESP32 Integration:** Utilizes the ESP32 development board for efficient Bluetooth communication and data management.

## Components

1. **ESP32 Development Board**: The core of the project, responsible for managing Bluetooth connections and data transfer.
2. **External Hard Drive**: The storage device that extends the phone's storage capacity.
3. **Mobile App**: A companion app developed to facilitate communication between the mobile phone and the hard drive.
4. **Power Supply**: Provides power to the ESP32 and the external hard drive.

## Installation

### Hardware Setup

1. **ESP32 Configuration**: Follow the instructions provided in the ESP32 documentation to set up the board.
2. **Connecting the Hard Drive**: Connect the external hard drive to the ESP32 using appropriate interfacing methods (e.g., USB or serial connection).
3. **Powering the System**: Ensure that both the ESP32 and the hard drive are properly powered.

### App Installation

1. **Download the App**: Search for “Graphite” on your mobile app store (available on both Android and iOS).
2. **Install the App**: Follow the on-screen instructions to complete the installation.

## Usage

1. **Open the App**: Launch the Graphite app on your mobile device.
2. **Connect to Hard Drive**: Use the app to search for and connect to the Bluetooth hard drive.
3. **Transfer Files**: Select files on your phone to transfer to the hard drive, or vice versa.
4. **Manage Storage**: Use the app’s interface to manage and organize files on the external hard drive.

## Technical Details

- **Development Board**: ESP32
- **Bluetooth Protocol**: BLE (Bluetooth Low Energy)
- **Mobile App**: Developed using React Native , Swift .
- **File Management**: Implemented with  SQLite



## Acknowledgements

- **ESP32 Development Board**: For providing the hardware foundation for the project.

## Document 

## **1. Project Objectives:**

- Create a functional system to connect an ESP32 to a phone via Bluetooth using React Native.
- Develop the capability to upload files (photos, documents) from the phone to an external hard drive.
- Use native modules to interact with phone storage and Bluetooth functionality in React Native.
- Ensure seamless data transfer and storage between the phone and the external drive.
  
---

## **2. System Architecture:**

### **2.1 ESP32 Bluetooth Module:**
- The ESP32 will act as the bridge between the phone and the hard drive.
- Bluetooth communication will be used to transfer files between the phone and ESP32.
  
### **2.2 React Native Mobile App:**
- The mobile app will allow users to:
  - Connect to the external Bluetooth device (ESP32).
  - Browse photos and documents on their phone.
  - Select files to upload to the external hard drive.

### **2.3 External Storage (Bluetooth Hard Drive):**
- The external storage will be a hard drive connected to the ESP32 via a Bluetooth module.
- File transfer will take place from the phone to the ESP32 and from the ESP32 to the hard drive.

---

## **3. Technology Stack:**

### **3.1 Hardware:**
- **ESP32:** Bluetooth communication, handling file transfer between phone and external hard drive.
- **Bluetooth Hard Drive:** External storage device connected to the ESP32.

### **3.2 Software:**
- **React Native:** Mobile app frontend for file management and Bluetooth communication.
- **Native Modules (React Native):** Native code to interact with Bluetooth, file system.
- **ESP-IDF (ESP32 Development Framework):** Firmware for Bluetooth and file handling on ESP32.

### **3.3 Protocols:**
- **Bluetooth (BLE or Classic):** Wireless communication between phone and ESP32.
- **File Transfer Protocol (FTP) or HTTP-based:** For managing file uploads.

---

## **4. Project Components & Features:**

### **4.1 ESP32 Module:**
- **Bluetooth Communication:** Handle pairing and communication with the mobile device.
- **File Transfer Protocol:** Use a lightweight protocol like FTP or HTTP to upload files from the phone to the hard drive.
- **Storage Management:** Ensure the hard drive can be recognized, mounted, and accessed by the ESP32.

### **4.2 React Native App:**
- **File Picker:** Implement a file picker allowing users to browse files (photos, documents) on their phone.
- **Bluetooth Connection:** Display nearby Bluetooth devices and allow the user to connect to the ESP32.
- **File Upload:** Integrate functionality for users to select files and upload them to the ESP32 via Bluetooth.
  
### **4.3 Native Modules (React Native):**
- **Bluetooth Module:** Use native code to handle low-level Bluetooth functionality.
- **File System Access:** Use platform-specific APIs to browse and manage the file system.
  
---

## **5. Functional Requirements:**

### **5.1 User Flow:**
1. **Connect Bluetooth Device:**
   - User opens the app and connects to the ESP32 Bluetooth device.
2. **Browse Phone Storage:**
   - User browses their phone's local storage and selects files (photos, documents).
3. **Upload Files:**
   - Selected files are sent from the phone to the ESP32, which then stores them on the connected hard drive.
4. **View Upload Status:**
   - User sees confirmation once files are successfully uploaded.

### **5.2 Performance Requirements:**
- Fast Bluetooth connection with low latency.
- Efficient file transfer without data loss or corruption.
- Support for large file uploads, depending on the hard drive size.

---

## **6. Non-Functional Requirements:**

- **Usability:** The app should have an intuitive interface for users to upload files easily.
- **Security:** Ensure secure data transfer over Bluetooth with encryption, if needed.
- **Scalability:** Future proofing the design for more complex storage devices or cloud-based integrations.
- **Reliability:** The system should handle dropped connections and large file uploads smoothly.
  
---

## **7. Development Plan:**

### **7.1 Phases:**

1. **Phase 1: Research and Prototyping**
   - Research ESP32 Bluetooth capabilities and how to interface with an external hard drive.
   - Study React Native’s Bluetooth and file system handling.

2. **Phase 2: ESP32 Development**
   - Set up the ESP32 as a Bluetooth module.
   - Implement a file transfer mechanism between ESP32 and external hard drive.

3. **Phase 3: React Native App**
   - Develop the UI to browse files and manage Bluetooth connections.
   - Create native modules to handle Bluetooth and file system interaction.

4. **Phase 4: Integration**
   - Connect the React Native app to ESP32 via Bluetooth and test file transfers.
   - Optimize file upload speed and reliability.

5. **Phase 5: Testing and Debugging**
   - Test the entire system for performance, usability, and edge cases (e.g., disconnections, large file uploads).
   - Conduct user acceptance testing.

### **7.2 Timeline:**

| Phase          | Start Date | End Date   | Duration |
|----------------|------------|------------|----------|
| Phase 1        | TBD        | TBD        | 1 week   |
| Phase 2        | TBD        | TBD        | 2 weeks  |
| Phase 3        | TBD        | TBD        | 3 weeks  |
| Phase 4        | TBD        | TBD        | 1 week   |
| Phase 5        | TBD        | TBD        | 1 week   |

---

## **8. Tools and Resources:**

### **8.1 Hardware:**
- ESP32 microcontroller.
- External hard drive.
- Bluetooth module (if not integrated with ESP32).

### **8.2 Software:**
- React Native framework.
- ESP-IDF for ESP32 firmware development.
- Xcode/Android Studio for mobile development.

---

## **9. Potential Challenges:**

- **Bluetooth Communication:** Ensuring reliable Bluetooth pairing and communication.
- **File Transfer Speed:** Handling large file uploads without delays or loss.
- **Cross-Platform Issues:** Dealing with differences in how iOS and Android handle Bluetooth and file systems.
- **ESP32 Storage Limitations:** Ensuring that the ESP32 can handle the file size and transfer to the hard drive efficiently.

---

## **10. Success Criteria:**

- Seamless Bluetooth connection between the phone and ESP32.
- Successful upload of files from the mobile device to the external hard drive.
- Intuitive and responsive user interface.
  

