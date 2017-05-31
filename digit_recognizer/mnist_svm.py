import numpy as np 
import pandas as pd
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt
from sklearn import svm

load_train_data = pd.read_csv("input/train.csv")
train_data = load_train_data.iloc[:,1:]
labels = load_train_data.iloc[:,0]

train_data, validation_data, train_labels,validation_labels = train_test_split(train_data,labels,train_size=0.8, random_state=0)
print(train_data.shape)
print(train_labels.shape)
print(validation_data.shape)
print(validation_labels.shape)

classifier = svm.SVC()
classifier.fit(train_data,train_labels)
classifier.score(validation_data,validation_labels)

train_data[train_data>0] = 1
validation_data[validation_data>0] = 1

classifier.fit(train_data,train_labels)
classifier.score(validation_data,validation_labels)

test_data = pd.read_csv("input/test.csv")
test_load_data[test_data>0] = 1
results= classifier.predict(test_data)

results = pd.DataFrame(results)
results.index.name = "ImageId"
results.index += 1
results.columns = ["Label"]
results.to_csv("results.csv",header=True)