import numpy as np
import csv
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report
from sklearn.svm import SVC

sites = {"AMZN":0,"BKNG":1, "CHESS":2, "DDG":3, "GIT":4, "GOGL":5, "TKTK":6, "WKPDA":7,"X":8,"YT":9, "YT-LQ":10}


X = np.zeros((110,375000))
x_ndx = 0
Y = np.zeros((110))


for site in sites.keys():
    for i in range(11):
        filename = "./"+site+"/"+str(i)+".txt"

        with open(filename, 'r') as read_obj: 
            csv_reader = csv.reader(read_obj) 
            list_of_csv = list(csv_reader) 
            
            arr = np.array(list_of_csv)
            arr = np.asarray(arr,dtype="int")
            arr = np.reshape(arr,375000)
            X[x_ndx] = np.copy(arr)
            Y[x_ndx] = sites[site]
            x_ndx+=1
        

X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.2, random_state=42)
print(X_train.shape)
print(X_test.shape)
print(Y_train.shape)
print(Y_test.shape)


rf = RandomForestClassifier(n_estimators=100, random_state=42) 


rf.fit(X_train, Y_train)

y_pred = rf.predict(X_test)


print("Accuracy:", accuracy_score(Y_test, y_pred))
print("\nClassification Report:\n", classification_report(Y_test, y_pred))

print("--------------SVM-----------------")

svm = SVC(kernel='linear', random_state=42)  
svm.fit(X_train, Y_train)


y_pred = svm.predict(X_test)


print("Accuracy:", accuracy_score(Y_test, y_pred))
print("Classification Report:", classification_report(Y_test, y_pred))


