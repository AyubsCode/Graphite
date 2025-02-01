import React, { useRef, useState } from "react";
import {
  View,
  Text,
  StyleSheet,
  TextInput,
  TouchableOpacity,
  SafeAreaView,
  Animated,
  PanResponder,
  Dimensions,
  ScrollView,
  Platform,
  KeyboardAvoidingView,
  TouchableWithoutFeedback,
} from "react-native";
import { Ionicons } from "@expo/vector-icons";
import { StatusBar } from "expo-status-bar";

interface FileItem {
  name: string;
  date: string;
}

interface AddFilesButtonProps {
  onPress: () => void;
}

const WINDOW_HEIGHT = Dimensions.get("window").height;
const WINDOW_WIDTH = Dimensions.get("window").width;
const BOTTOM_SHEET_MIN_HEIGHT = WINDOW_HEIGHT * 0.5;
const BOTTOM_SHEET_MAX_HEIGHT = WINDOW_HEIGHT * 0.9;
const STORAGE_SECTION_HEIGHT = 400;
const DRAWER_WIDTH = WINDOW_WIDTH * 0.8;

// Add Files Button Component
const AddFilesButton: React.FC<AddFilesButtonProps> = ({ onPress }) => {
  return (
    <View style={styles.addButtonContainer}>
      <TouchableOpacity style={styles.addButton} onPress={onPress}>
        <Ionicons name="add" size={24} color="white" />
        <Text style={styles.addButtonText}>Add files</Text>
      </TouchableOpacity>
    </View>
  );
};

// Drawer Menu Component
const DrawerMenu: React.FC<{
  isVisible: boolean;
  translateX: Animated.Value;
  onClose: () => void;
}> = ({ isVisible, translateX, onClose }) => {
  const menuItems = [
    { icon: "timer-outline", label: "Recent" },
    { icon: "star-outline", label: "Starred" },
    { icon: "folder-outline", label: "Folders" },
    { icon: "trash-outline", label: "Trash" },
    { icon: "settings-outline", label: "Settings" },
    { icon: "help-circle-outline", label: "Help & Feedback" },
  ];

  return (
    <>
      {isVisible && (
        <TouchableWithoutFeedback onPress={onClose}>
          <Animated.View style={[styles.backdrop, { opacity: 0.5 }]} />
        </TouchableWithoutFeedback>
      )}
      <Animated.View
        style={[
          styles.drawer,
          {
            transform: [{ translateX }],
            display: isVisible ? "flex" : "none",
          },
        ]}>
        <SafeAreaView style={styles.drawerContent}>
          <Text style={styles.drawerTitle}>Graphite</Text>
          {menuItems.map((item, index) => (
            <TouchableOpacity key={index} style={styles.drawerItem}>
              <Ionicons
                name={item.icon as keyof (typeof Ionicons)["glyphMap"]}
                size={24}
                color="#4285f4"
              />
              <Text style={styles.drawerItemText}>{item.label}</Text>
            </TouchableOpacity>
          ))}
        </SafeAreaView>
      </Animated.View>
    </>
  );
};

export default function HomeScreen() {
  const totalSpace = 150;
  const usedSpace = 102;
  const usagePercentage = Math.round((usedSpace / totalSpace) * 100);
  const pan = useRef(new Animated.ValueXY()).current;
  const [isDrawerVisible, setIsDrawerVisible] = useState(false);
  // const drawerTranslateX = useRef(new Animated.Value(-DRAWER_WIDTH)).current;
  const drawerTranslateX = useRef(
    new Animated.Value(-WINDOW_WIDTH * 0.8)
  ).current;

  const toggleDrawer = () => {
    const toValue = isDrawerVisible ? -WINDOW_WIDTH * 0.8 : 0;
    Animated.timing(drawerTranslateX, {
      toValue,
      duration: 300,
      useNativeDriver: true,
    }).start();
    setIsDrawerVisible(!isDrawerVisible);
  };

  const closeDrawer = () => {
    Animated.timing(drawerTranslateX, {
      toValue: -WINDOW_WIDTH * 0.8,
      duration: 300,
      useNativeDriver: true,
    }).start(() => setIsDrawerVisible(false));
  };

  const handleAddFiles = () => {
    // Implement add files logic
    console.log("Add files pressed");
  };

  const panResponder = PanResponder.create({
    onStartShouldSetPanResponder: () => true,
    onPanResponderMove: (_, gesture) => {
      const newY = gesture.dy;
      if (
        newY <= 0 &&
        newY >= -(BOTTOM_SHEET_MAX_HEIGHT - BOTTOM_SHEET_MIN_HEIGHT)
      ) {
        pan.setValue({ x: 0, y: newY });
      }
    },
    onPanResponderRelease: (_, gesture) => {
      if (gesture.dy < -50) {
        Animated.spring(pan, {
          toValue: {
            x: 0,
            y: -(BOTTOM_SHEET_MAX_HEIGHT - BOTTOM_SHEET_MIN_HEIGHT),
          },
          useNativeDriver: false,
        }).start();
      } else if (gesture.dy > 50) {
        Animated.spring(pan, {
          toValue: { x: 0, y: 0 },
          useNativeDriver: false,
        }).start();
      } else {
        const shouldGoUp =
          -gesture.y0 >
          BOTTOM_SHEET_MIN_HEIGHT +
            (BOTTOM_SHEET_MAX_HEIGHT - BOTTOM_SHEET_MIN_HEIGHT) / 2;
        Animated.spring(pan, {
          toValue: {
            x: 0,
            y: shouldGoUp
              ? -(BOTTOM_SHEET_MAX_HEIGHT - BOTTOM_SHEET_MIN_HEIGHT)
              : 0,
          },
          useNativeDriver: false,
        }).start();
      }
    },
  });

  const files: FileItem[] = [
    { name: "ReportPaper_1.pdf", date: "Jan 26, 2025" },
    { name: "ReportPaper_1.pdf", date: "Jan 26, 2025" },
    { name: "ReportPaper_1.pdf", date: "Jan 26, 2025" },
  ];

  return (
    <KeyboardAvoidingView
      behavior={Platform.OS === "ios" ? "padding" : "height"}
      style={styles.container}>
      <StatusBar style="dark" />
      <StatusBar style="dark" />
      <DrawerMenu
        isVisible={isDrawerVisible}
        translateX={drawerTranslateX}
        onClose={closeDrawer}
      />

      <DrawerMenu
        isVisible={isDrawerVisible}
        translateX={drawerTranslateX}
        onClose={closeDrawer}
      />

      <SafeAreaView style={styles.mainContent}>
        <View style={styles.header}>
          <TouchableOpacity onPress={toggleDrawer} style={styles.menuButton}>
            <Ionicons name="menu" size={24} color="black" />
          </TouchableOpacity>
        </View>

        <View style={styles.storageSection}>
          <View style={styles.circleContainer}>
            <View style={styles.circle}>
              <Text style={styles.percentageText}>{usagePercentage}%</Text>
              <Text style={styles.usageText}>Used space</Text>
            </View>
          </View>

          <View style={styles.storageDetails}>
            <View style={styles.storageItem}>
              <View style={styles.totalSpaceIndicator} />
              <Text style={styles.storageLabel}>Total space</Text>
              <Text style={styles.storageValue}>{totalSpace} GB</Text>
            </View>
            <View style={styles.storageItem}>
              <View style={styles.usedSpaceIndicator} />
              <Text style={styles.storageLabel}>Space used</Text>
              <Text style={styles.storageValue}>{usedSpace} GB</Text>
            </View>
          </View>
        </View>
      </SafeAreaView>

      <Animated.View
        style={[
          styles.bottomSheet,
          {
            transform: [{ translateY: pan.y }],
            height: BOTTOM_SHEET_MAX_HEIGHT,
            top: WINDOW_HEIGHT - BOTTOM_SHEET_MIN_HEIGHT,
          },
        ]}>
        <View style={styles.dragHandle} {...panResponder.panHandlers}>
          <View style={styles.dragIndicator} />
        </View>

        <View style={styles.bottomSheetContent}>
          <View style={styles.searchContainer}>
            <Ionicons name="search" size={20} color="#666" />
            <TextInput
              style={styles.searchInput}
              placeholder="Search for files"
              placeholderTextColor="#666"
            />
          </View>

          <View style={styles.categoryContainer}>
            <TouchableOpacity style={styles.categoryButton}>
              <Ionicons name="images-outline" size={20} color="#4285f4" />
              <Text style={styles.categoryText}>Images</Text>
            </TouchableOpacity>
            <TouchableOpacity style={styles.categoryButton}>
              <Ionicons name="videocam-outline" size={20} color="#4285f4" />
              <Text style={styles.categoryText}>Videos</Text>
            </TouchableOpacity>
            <TouchableOpacity style={styles.categoryButton}>
              <Ionicons name="document-outline" size={20} color="#4285f4" />
              <Text style={styles.categoryText}>Documents</Text>
            </TouchableOpacity>
          </View>

          <ScrollView
            style={styles.fileList}
            contentContainerStyle={styles.fileListContent}>
            {files.map((file, index) => (
              <View key={index} style={styles.fileItem}>
                <View style={styles.fileInfo}>
                  <Ionicons name="document-outline" size={24} color="#4285f4" />
                  <View style={styles.fileDetails}>
                    <Text style={styles.fileName}>{file.name}</Text>
                    <Text style={styles.fileDate}>Imported • {file.date}</Text>
                  </View>
                </View>
                <TouchableOpacity>
                  <Ionicons name="ellipsis-horizontal" size={24} color="#666" />
                </TouchableOpacity>
              </View>
            ))}
          </ScrollView>
        </View>
      </Animated.View>

      <AddFilesButton onPress={handleAddFiles} />
    </KeyboardAvoidingView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#fff",
  },
  mainContent: {
    height: STORAGE_SECTION_HEIGHT,
    paddingHorizontal: 20,
  },
  header: {
    paddingTop: Platform.OS === "ios" ? 20 : 40,
  },
  storageSection: {
    flex: 1,
    justifyContent: "center",
    paddingBottom: 20,
  },
  circleContainer: {
    alignItems: "center",
    marginVertical: 20,
  },
  circle: {
    width: 200,
    height: 200,
    borderRadius: 100,
    borderWidth: 20,
    borderColor: "#4285f4",
    alignItems: "center",
    justifyContent: "center",
    borderRightColor: "#E1F5FE",
    // transform: [{ rotate: "90deg" }],
  },
  percentageText: {
    fontSize: 36,
    fontWeight: "bold",
    transform: [{ rotate: "-135deg" }],
  },
  usageText: {
    color: "#666",
    transform: [{ rotate: "-135deg" }],
  },
  storageDetails: {
    flexDirection: "row",
    justifyContent: "space-around",
    marginTop: 20,
  },
  storageItem: {
    alignItems: "center",
  },
  storageLabel: {
    color: "#666",
    fontSize: 12,
    marginTop: 5,
  },
  totalSpaceIndicator: {
    width: 12,
    height: 12,
    backgroundColor: "#E1F5FE",
    borderRadius: 6,
  },
  usedSpaceIndicator: {
    width: 12,
    height: 12,
    backgroundColor: "#4285f4",
    borderRadius: 6,
  },
  storageValue: {
    fontWeight: "bold",
    marginTop: 2,
  },
  bottomSheet: {
    position: "absolute",
    left: 0,
    right: 0,
    backgroundColor: "#fff",
    borderTopLeftRadius: 20,
    borderTopRightRadius: 20,
    shadowColor: "#000",
    shadowOffset: {
      width: 0,
      height: -4,
    },
    shadowOpacity: 0.1,
    shadowRadius: 4,
    elevation: 5,
  },
  dragHandle: {
    width: "100%",
    height: 30,
    alignItems: "center",
    justifyContent: "center",
  },
  dragIndicator: {
    width: 40,
    height: 4,
    backgroundColor: "#DEDEDE",
    borderRadius: 2,
  },
  bottomSheetContent: {
    flex: 1,
    padding: 20,
  },
  searchContainer: {
    flexDirection: "row",
    alignItems: "center",
    backgroundColor: "#f5f5f5",
    borderRadius: 10,
    padding: 10,
    marginBottom: 20,
  },
  searchInput: {
    flex: 1,
    marginLeft: 10,
  },
  categoryContainer: {
    flexDirection: "row",
    justifyContent: "space-between",
    marginBottom: 20,
  },
  categoryButton: {
    flexDirection: "row",
    alignItems: "center",
    padding: 10,
    borderRadius: 8,
    backgroundColor: "#F5F5F5",
  },
  categoryText: {
    color: "#4285f4",
    marginLeft: 5,
    fontWeight: "500",
  },
  fileList: {
    flex: 1,
  },
  fileListContent: {
    paddingBottom: 100,
  },
  fileItem: {
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "space-between",
    paddingVertical: 15,
    borderBottomWidth: 1,
    borderBottomColor: "#f0f0f0",
  },
  fileInfo: {
    flexDirection: "row",
    alignItems: "center",
  },
  fileDetails: {
    marginLeft: 15,
  },
  fileName: {
    fontWeight: "500",
  },
  fileDate: {
    color: "#666",
    fontSize: 12,
    marginTop: 2,
  },
  addButtonContainer: {
    position: "absolute",
    bottom: 0,
    left: 0,
    right: 0,
    padding: 20,
    backgroundColor: "white",
    borderTopWidth: 1,
    borderTopColor: "#f0f0f0",
  },
  addButton: {
    flexDirection: "row",
    alignItems: "center",
    justifyContent: "center",
    backgroundColor: "#4285f4",
    borderRadius: 25,
    padding: 15,
  },
  addButtonText: {
    color: "white",
    marginLeft: 10,
    fontWeight: "500",
  },
  menuButton: {
    marginLeft: 20,
  },

  backdrop: {
    ...StyleSheet.absoluteFillObject,
    backgroundColor: "black",
    zIndex: 999,
  },
  drawer: {
    position: "absolute",
    left: 0,
    top: 0,
    bottom: 0,
    width: WINDOW_WIDTH * 0.8,
    backgroundColor: "white",
    zIndex: 1000,
    elevation: 5,
    shadowColor: "#000",
    shadowOffset: {
      width: 2,
      height: 0,
    },
    shadowOpacity: 0.25,
    shadowRadius: 3.84,
  },
  drawerContent: {
    flex: 1,
    paddingTop: Platform.OS === "android" ? 50 : 0,
  },
  drawerTitle: {
    fontSize: 24,
    fontWeight: "bold",
    color: "#4285f4",
    padding: 20,
  },
  drawerItem: {
    flexDirection: "row",
    alignItems: "center",
    padding: 15,
    paddingLeft: 20,
  },
  drawerItemText: {
    marginLeft: 15,
    fontSize: 16,
    color: "#4285f4",
  },
});
