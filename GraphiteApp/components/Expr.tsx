import { Alert , View, Text, StyleSheet  , Button } from "react-native" ;
import * as DocumentPicker from 'expo-document-picker'          ;
import * as ImagePicker from 'expo-image-picker'          ;

/* TODO : Make it so that whether or not the user has allowed access media files occurs on startup or when pairing device . 
 * 
 *
 *
 *
 */

interface fileHandler {
  bar : () => void ; 
}

const handleUpload : fileHandler = { 
  bar : () => {
    console.log("Handling Add File") ; 
  }
}

const handleProcess : fileHandler = { 
  bar : () => {
    console.log("Handling Process") ; 
  }
}


// Wrap in try catch later

const pickFile = async() => {
  console.log("addfile Pressed") ;
  // const permission = ImagePicker.requestMediaLibraryPermissionsAsync() ; 
  // if(!permissions.granted){
  //   Alert("File Access Required") ; 
  //   return ; 
  // }
  const result = ImagePicker.launchImageLibraryAsync({
      mediaTypes: ImagePicker.MediaTypeOptions.All, 
      allowsEditing: true,
      quality: 1, // quality
  }); 
  if ( result.cancelled ) return ; 
  console.log(`Values: ${result}` )
  return result[0] ; 
}

export default function Expr() {
  return (
    <View style={styles.container}>
      <Text>EXPR SCREEN</Text>
      <Button onPress={pickFile} title="Add File"></Button>
      <Button onPress={handleProcess.bar} title="Add Process"></Button>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#fff",
    alignItems: "center",
    justifyContent: "center",
  },
});
