import React from 'react'
import * as SQLite from "expo-sqlite"

// Typeset sqlite tables and functions
export interface File {
  file_id: number;
  file_name: string;
  file_size: number;
  extension: string;
  creation_time: string;
  last_access: string;
  permissions: string;
}

export interface User {
  user_id: number;
  username: string;
  password: string;
  email: string;
  first_name: string;
  last_name: string;
}

type setUserFunc = (users: User[]) => void;
type setFileFunc = (files: File[]) => void;
type successFunc = () => void;


// Create instance of Database
let db: SQLite.SQLiteDatabase | null = null;

async function initializeDb(): Promise<SQLite.SQLiteDatabase> {
  if (!db) {
    db = await SQLite.openDatabaseAsync('graphite.db');
  }
  return db;
}
  
const initializeDatabase = async (): Promise<void>  => {
  try {
    const db = await initializeDb();
    await db.execAsync(
      `CREATE TABLE IF NOT EXISTS Files (
        file_id INTEGER PRIMARY KEY AUTOINCREMENT,
        file_name TEXT NOT NULL,
        file_size INTEGER NOT NULL,
        extension TEXT NOT NULL,
        creation_time TEXT NOT NULL,
        last_access TEXT,
        permissions TEXT,
      );
    
      CREATE TABLE IF NOT EXISTS Users (
        user_id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT NOT NULL UNIQUE,
        password TEXT NOT NULL,
        email TEXT NOT NULL UNIQUE,
        first_name TEXT NOT NULL,
        last_name TEXT NOT NULL
      );

      CREATE TABLE IF NOT EXISTS Roles (
        role_id INTEGER PRIMARY KEY AUTOINCREMENT,
        role_name TEXT NOT NULL UNIQUE
      );

      CREATE TABLE IF NOT EXISTS UserRole (
        user_id INTEGER NOT NULL,
        role_id INTEGER NOT NULL,
        PRIMARY KEY (user_id, role_id),
        FOREIGN KEY (user_id) REFERENCES Users(user_id) ON DELETE CASCADE ON UPDATE CASCADE,
        FOREIGN KEY (role_id) REFERENCES Roles(role_id) ON DELETE CASCADE ON UPDATE CASCADE
      );

      CREATE TABLE IF NOT EXISTS Directory (
        directory_id INTEGER PRIMARY KEY AUTOINCREMENT,
        directory_name TEXT NOT NULL,
        directory_size INTEGER NOT NULL DEFAULT 0,
        number_of_files INTEGER NOT NULL DEFAULT 0,
        creation_time TEXT NOT NULL,
        last_access TEXT,
        permissions TEXT,
        parent_directory INTEGER DEFAULT NULL,
        FOREIGN KEY (parent_directory) REFERENCES Directory(directory_id) ON DELETE SET NULL ON UPDATE CASCADE
      );

      CREATE TABLE IF NOT EXISTS FileLocation (
        file_id INTEGER NOT NULL,
        directory_id INTEGER NOT NULL,
        path TEXT NOT NULL,
        previous_location INTEGER DEFAULT NULL,
        PRIMARY KEY (file_id, directory_id),
        FOREIGN KEY (file_id) REFERENCES Files(file_id) ON DELETE CASCADE ON UPDATE CASCADE,
        FOREIGN KEY (directory_id) REFERENCES Directory(directory_id) ON DELETE CASCADE ON UPDATE CASCADE,
        FOREIGN KEY (previous_location) REFERENCES Directory(directory_id) ON DELETE SET NULL ON UPDATE CASCADE
        );
    `);
  } catch (error) {
    console.log('db error creating tables');
    console.log(error);
    throw error;
  }
};
 

const emptyDatabase = async (): Promise<void> => {
  try {
    const db = await initializeDb();
    await db.execAsync(
      `DROP TABLE Files;
      DROP TABLE Users;
      DROP TABLE Roles;
      DROP TABLE UserRole;
      DROP TABLE Directory;
      DROP TABLE FileLocation; `
      );
  } catch (error) {
    console.log("Failed to empty database."); 
    console.log(error); 
    throw(error);
  }      
};


// Test Data
// --------------------------------------------------------

const loadUsers = async (): Promise<void> => {
  let statement; // Declare statement outside try block

  try {
    const db = await initializeDb();
    statement = await db.prepareAsync(
      'INSERT INTO Users (username, password, email, first_name, last_name) VALUES ($username, $password, $email, $first_name, $last_name)'
    );

    await statement.executeAsync({
      $username: "john",
      $password: "pass123",
      $email: "john@gmail.com",
      $first_name: "john",
      $last_name: "doe"
    });

    await statement.executeAsync({
      $username: "jane",
      $password: "pass456",
      $email: "jane@gmail.com",
      $first_name: "jane",
      $last_name: "johnson"
    });

  } catch (error) {
    console.error("Error loading users:", error);
    throw error;
  } finally {
    if (statement) {
      await statement.finalizeAsync().catch(err => 
        console.error("Error loading users:", err)
      );
    }
  }
};
      

// Get functions
// --------------------------------------------------------

const getFiles = async (setFileFunc: setFileFunc): Promise<void> => {
  try {
    const db = await initializeDb();
    const results = await db.getAllAsync<File>('SELECT * FROM Files;');
    setFileFunc(results);
    console.log('loaded files');
  } catch (error) {
    console.log('db error load files');
    console.log(error);
    throw error;
  }
};


const getUsers = async (setUserFunc: setUserFunc): Promise<void> => {
  try {
    const database = await initializeDb();
    const results = await database.getAllAsync<User>('SELECT * FROM Users;');
    setUserFunc(results);
    console.log('loaded users');
  } catch (error) {
    console.log('db error load users');
    console.log(error);
    throw error;
  }
};

// Set functions
// --------------------------------------------------------

const setFiles = async (file_name: string, file_size: number, extension: string, creation_time: string, last_access: string, permissions: string, successFunc: successFunc): Promise<void> => {
  try {
    const db = await initializeDb();
    await db.runAsync(
      'INSERT INTO Files (file_name, file_size, extension, creation_time, last_access, permissions) VALUES (?, ?, ?, ?, ?, ?)',
      [file_name, file_size, extension, creation_time, last_access, permissions]);
    successFunc();
} catch (error) {
  console.log('db error set file');
  console.log(error);
  throw error;
  }
};


const setUser = async (username: string, password: string, email: string, first_name: string, last_name: string, successFunc: successFunc): Promise<void> => {
  try {
    const db = await initializeDb();
      await db.runAsync(
      'INSERT INTO Users (username, password, email, first_name, last_name) VALUES (?, ?, ?, ?, ?)',
      [username, password, email, first_name, last_name] );
  } catch (error) {
    console.log('db error set user');
    console.log(error);
    throw error;
    }
  };



export const Queries = {
  initializeDatabase,
  emptyDatabase,
  loadUsers,
  getFiles,
  getUsers,
  setFiles,
  setUser
};
    
