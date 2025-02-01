import { Alert , View, Text, StyleSheet  , Button } from "react-native" ;
import * as DocumentPicker from 'expo-document-picker'          ;
import * as ImagePicker from 'expo-image-picker'          ;

/* TODO : Make it so that whether or not the user has allowed access media files occurs on startup or when pairing device . 
/* TODO : is assetId needed ?  ln 37
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

type Dimensions = {
  height : number ,  
  width  : number ,  
}


type Image = {
  fileName   : string        ; // files name
  size       : number        ; // files size
  assetID    : string        ; // idk what this is or whether we should add it 
  dimensions : Dimensions    ;
  type       : string        ;
}

type Video = {

}

type Document = {

}




// Wrap in try catch later

const getFile = async () => {
  const permission = await ImagePicker.requestMediaLibraryPermissionsAsync();
  if (!permission.granted) {
    alert("Permission to access media library is required!");
    return;
  }

  const result = await ImagePicker.launchImageLibraryAsync({
    mediaTypes: ImagePicker.MediaTypeOptions.All, // Images & Videos
    allowsEditing: true,
    quality: 1, // High quality
  });

  if (!result.canceled) {
    console.log("Picked file:", result.assets[0]);
    return result.assets[0]; // File object
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
