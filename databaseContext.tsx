import React, { useEffect, createContext, useState, useContext, useCallback, ReactElement} from 'react';
import {Queries, User, File} from '../components/SQLiteHelper'

type DataContextType   = {
    files: File[];
    users: User[];
    addFile: (file_name: string, file_size: number, extension: string, creation_time: string, last_access: string, permissions: string) => Promise<void>;
    addUser: (username: string, password: string, email: string, first_name: string, last_name: string) => Promise<void>;
    refreshUsers: () => Promise<void>;
    refreshFiles: () => Promise<void>;
    isLoading: boolean;
  };

const DataContext = createContext<DataContextType>({} as DataContextType);

type DataProviderProps  = {
    children: React.ReactNode;
    initialUsers?: User[];
    initialFiles?: File[];
};

export const DataProvider: React.FC<DataProviderProps> = ({    children,
    initialUsers = [],
    initialFiles = []
  }): ReactElement=> {
    const [users, setUsersContext] = useState<User[]>(initialUsers);
    const [files, setFilesContext] = useState<File[]>(initialFiles);
    const [isLoading, setIsLoadingContext] = useState(true);

    const initializeDatabase = useCallback(async () => {
        try {
            await Queries.initializeDatabase()
            await Queries.loadUsers()
        } catch (err) {
          console.error('Database initialization error:', err);
        }
      }, []);


    const refreshUsers = useCallback(async () => {
        try {
          setIsLoadingContext(true);
          await Queries.getUsers((loadedUsers) => {
            setUsersContext(loadedUsers);
            console.log('Users refreshed');
          });
        } catch (err) {
          console.error('Failed to refresh users:', err);
          throw err;
        } finally {
          setIsLoadingContext(false);
        }
      }, []);


    const refreshFiles = useCallback(async () => {
        try {
          setIsLoadingContext(true);
          await Queries.getFiles((loadedFiles) => {
            setFilesContext(loadedFiles);
            console.log('Files refreshed');
          });
        } catch (err) {
          console.error('Failed to refresh files:', err);
          throw err;
        } finally {
          setIsLoadingContext(false);
        }
      }, []);


    const addFile = useCallback(async (file_name: string, file_size: number, extension: string, creation_time: string, last_access: string, permissions: string) => {
      try {
        setIsLoadingContext(true);
        await Queries.setFiles(file_name, file_size, extension, creation_time, last_access, permissions, async () => {
          console.log('File inserted, refreshing...');
          await refreshFiles();
        });
      } catch (err) {
        console.error('Failed to add file:', err);
        throw err;
      } finally {
        setIsLoadingContext(false);
      }
    }, [refreshFiles]);


    const addUser = useCallback(async (username: string, password: string, email: string, first_name: string, last_name: string) => {
      try {
        setIsLoadingContext(true);
        await Queries.setUser(username, password, email, first_name, last_name, async () => {
          console.log('User inserted, refreshing...');
          await refreshUsers();
        });
      } catch (err) {
        console.error('Failed to add user:', err);
        throw err;
      } finally {
        setIsLoadingContext(false);
      }
    }, [refreshUsers]);

    useEffect(() => {
    const initialize = async () => {
      try {
        await initializeDatabase();
        await Promise.all([refreshUsers(), refreshFiles()]);
      } catch (err) {
        console.error('Initialization error:', err);
        throw err;
      }
    };
    
    initialize();
  }, [initializeDatabase, refreshUsers, refreshFiles]);


  const contextValue: DataContextType = {
    users,
    files,
    addUser,
    addFile,
    refreshUsers,
    refreshFiles,
    isLoading,
  };

  return (
    <DataContext.Provider value={contextValue}>
      {children}
    </DataContext.Provider>
  );
};

export const useData = (): DataContextType => {
  const context = useContext(DataContext);
  if (!context) {
    throw new Error('Use DataProvider in app to load data');
  }
  return context;
};

