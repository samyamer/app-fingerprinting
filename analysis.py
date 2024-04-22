import numpy as np
import csv
from sklearn.model_selection import train_test_split
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import LSTM, Dense

sites = {"AMZN":0,"BKNG":1, "CHESS":2, "DDG":3, "GIT":4, "GOGL":5, "TKTK":6, "WKPDA":7,"X":8,"YT":9}


# with open("./AMZN/0.txt", 'r') as read_obj: 
#         csv_reader = csv.reader(read_obj) 
#         list_of_csv = list(csv_reader) 
        
#         arr = np.array(list_of_csv)
#         arr = np.asarray(arr,dtype="int")
#         X[0] = arr

# print(len(list_of_csv))

# print(type(list_of_csv[0][0]))

# print(len(arr))
# print(arr[0])
# print(type(arr[0][0]))
# print(arr.shape)

X = np.zeros((110,25000,15))
x_ndx = 0
Y = np.array((110))

for site in sites.keys():
    for i in range(11):
        filename = "./"+site+"/"+str(i)+".txt"

        with open(filename, 'r') as read_obj: 
            csv_reader = csv.reader(read_obj) 
            list_of_csv = list(csv_reader) 
            
            arr = np.array(list_of_csv)
            arr = np.asarray(arr,dtype="int")
            X[x_ndx] = np.copy(arr)
            Y[x_ndx] = sites[site]
            x_ndx+=1
        

X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.2, random_state=42)
print(X_train.shape)
print(X_test.shape)
print(Y_train.shape)
print(Y_test.shape)