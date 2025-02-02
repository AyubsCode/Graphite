import { Alert , View, Text, StyleSheet  , Button } from "react-native" ;
import * as DocumentPicker from 'expo-document-picker'          ;
import * as ImagePicker from 'expo-image-picker'          ;

/* TODO : Make it so that whether or not the user has allowed access media files occurs on startup or when pairing device . 
/* TODO : is assetId needed ?  ln 37
/* TODO : Add default path to filePath in the types . 
/* TODO : Fix file extension logic so it doesn't just map to something generic like " IMAGE "  , more granular like ".png" 
 */



/*
{
  "assetId": "F6F9AFD7-4373-4307-910A-F0BCE5FED50C/L0/001",
  "base64": null,
  "duration": null,
  "exif": null,
  "fileName": "IMG_4663.PNG",
  "fileSize": 1789453,
  "height": 1125,
  "mimeType": "image/png",
  "pairedVideoAsset": null,
  "type": "image",
  "uri": "file:///var/mobile/Containers/Data/Application/F24429FE-1B70-4AAD-8F57-D1D247C3202D/Library/Caches/ExponentExperienceData/@anonymous/GraphiteApp-ecbcb476-4904-4499-abb0-2d74727cc3c2/ImagePicker/B119EF0A-B2F5-407E-A405-60844A9973EE.png",
  "width": 1125
}
 * */


enum ERROR {
  STATUS_SUCCESS , 
  PARSE_ERROR    , 
}

type ParseCallback = ( fileDetails : Object ) => ERROR ;

type Dimensions = {
  height : number ,  
  width  : number ,  
}

type Image = {
  fileName      : string        ; // files name
  filePath      : string        ; // Add default path 
  size          : number        ; // files size
  assetID       : string        ; // idk what this is or whether we should add it 
  dimensions    : Dimensions    ;
  type          : string        ;
  fileExtension : string        ; // File type  idk if this is worth storing tho 
}

type Video = {

}
type Document = {

}


const parseVideo = ( fileDetails : object ) : ERROR => {
  console.log(`Attempting to parse video : [ ]`) ; 
  return STATUS_SUCCESS ; 
}

const parseImage = ( fileDetails : object ) : ERROR => {
  console.log(`Attempting to parse Image : [ ]`) ; 
  return STATUS_SUCCESS ; 
}

const parseDocument = ( fileDetails : object ) : ERROR => {
  console.log(`Attempting to parse Document : [ ]`) ; 
  return STATUS_SUCCESS ; 
}

let CALLBACK_MAP: Map<string, ParseCallback> = new Map([
  ["JPEG", parseImage],
  ["PNG", parseImage],
  ["image", parseImage],
  ["MP4", parseVideo],
  ["AVI", parseVideo],
  ["H.264", parseVideo],
  ["PDF", parseDocument],
  ["TXT", parseDocument],
]);




// Might need metadata later





// Wrap in try catch later
// Convert to type safe  ? 

const getFile = async() => {
  const permission = await ImagePicker.requestMediaLibraryPermissionsAsync();
  if (!permission.granted) {
    alert("Permission to access media library is required!");
    return;
  }

  const result = await ImagePicker.launchImageLibraryAsync({
    mediaTypes: ImagePicker.MediaTypeOptions.All,
    allowsEditing: true,
    quality: 1,
  });

  if (!result.canceled && result.assets[0]) {
    const fileType = result.assets[0].type;
    const execution_function = CALLBACK_MAP.get(fileType);
    if (!execution_function) {
      console.log("No handler found for file type:", fileType);
      return;
    }
    
    const parse_result = execution_function(result.assets[0]);
    if(result.assets[0].fileExtension) console.log("Not a thing") ; 
  }
};



export default function Expr() {
  return (
    <View style={styles.container}>
      <Text>EXPR SCREEN</Text>
      <Button onPress={getFile} title="Add File"></Button>
      <Button onPress={console.log("Press Handled")} title="Add Process"></Button>
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
