import csv
import matplotlib.pyplot as plt
import numpy as np

website = "GOGL"

for v in range(10):
    filename = "./" + website + "/" + str(v) + ".txt"
    with open(filename, 'r') as read_obj: 
        csv_reader = csv.reader(read_obj) 
        list_of_csv = list(csv_reader) 
    x=[]
    y=[]
    z=[]
    colors=[]



    for i in range(len(list_of_csv)):
        for j in range(len(list_of_csv[i])):
            x.append(i)
            y.append(j)
            if(list_of_csv[i][j] == '0'):
                colors.append('red')
            else:
                colors.append('yellow')
            #    z.append(list_of_csv[i][j])

    out = website+"_"+str(v)+".png"
    plt.figure(figsize=(50, 5))
    plt.scatter(x, y, c=colors,marker='s')
    plt.axis('off')
    plt.savefig(out, bbox_inches='tight')