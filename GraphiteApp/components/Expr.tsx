import { View, Text, StyleSheet  , Button } from "react-native";

export default function Expr() {
  return (
    <View style={styles.container}>
      <Text>EXPR SCREEN</Text>
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
