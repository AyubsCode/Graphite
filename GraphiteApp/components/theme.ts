
import { createContext, useContext } from "react";

export interface ThemeColors {
  primary: string;
  background: string;
  surface: string;
  text: string;
  textSecondary: string;
  border: string;
  accent: string;
  error: string;
  success: string;
  divider: string;
  inputBackground: string;
  shadow: string;
}

export const lightTheme: ThemeColors = {
  primary: "#4285f4",
  background: "#ffffff",
  surface: "#ffffff",
  text: "#000000",
  textSecondary: "#666666",
  border: "#f0f0f0",
  accent: "#E1F5FE",
  error: "#B00020",
  success: "#4CAF50",
  divider: "#DEDEDE",
  inputBackground: "#f5f5f5",
  shadow: "#000000",
};

export const darkTheme: ThemeColors = {
  primary: "#4285f4",
  background: "#121212",
  surface: "#1E1E1E",
  text: "#ffffff",
  textSecondary: "#B0B0B0",
  border: "#2C2C2C",
  accent: "#1A3A5A",
  error: "#CF6679",
  success: "#4CAF50",
  divider: "#2C2C2C",
  inputBackground: "#2C2C2C",
  shadow: "#000000",
};

interface ThemeContextType {
  theme: ThemeColors;
  isDark: boolean;
  toggleTheme: () => void;
}

export const ThemeContext = createContext<ThemeContextType>({
  theme: lightTheme,
  isDark: false,
  toggleTheme: () => {},
});

export const useTheme = () => useContext(ThemeContext);
