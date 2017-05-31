#Load libraries
import pandas as pd
import numpy as np

from sklearn.model_selection import train_test_split

from keras.models import Sequential
from keras.layers import Convolution2D
from keras.layers import MaxPooling2D
from keras.layers import Flatten
from keras.layers import Dense
from keras.preprocessing.image import ImageDataGenerator

#read training data
train_load_data = pd.read_csv("input/train.csv")
print("train data shape:",train_load_data.shape)

#read test data
test_data = pd.read_csv("input/test.csv")
print("test data shape:",test_data.shape)

data = train_load_data.iloc[:,1:].values
data_labels = train_load_data.iloc[:,0].values
test_data = test_data.values
#reshaping to numpy 3d array 
data = data.reshape(data.shape[0],28,28,1)
test_data = test_data.reshape(test_data.shape[0],28,28,1)

#zero mean and unit standard deviation
data_mean = data.mean().astype('float32')
data_sd = data.std().astype('float32')
data = data-data_mean
data = data/data_sd
test_data = test_data - data_mean
test_data = test_data/data_sd

#Generate training data and validation data
X_train,X_validation,y_train,y_validation = train_test_split(data,data_labels,train_size=0.8,random_state=0)

#cnn architecture 
classifier = Sequential()
#Convolution Layer
classifier.add(Convolution2D(filters=32,kernel_size=(3,3),input_shape=(28,28,1),activation='relu'))
#Pooling Layer
classifier.add(MaxPooling2D(pool_size=(2,2)))

#Second Convolution Layer
classifier.add(Convolution2D(filters=32,kernel_size=(3,3),activation='relu'))
#Pooling Layer
classifier.add(MaxPooling2D(pool_size=(2,2)))

classifier.add(Flatten())
#Fully connected layers
classifier.add(Dense(units=128,activation="relu"))
classifier.add(Dense(units=10,activation="softmax"))

classifier.compile(optimizer='adam',loss='sparse_categorical_crossentropy',metrics=['accuracy'])

#Data Augmentation to prevent overfitting ang generalize model
datagen = ImageDataGenerator()

train_set = datagen.flow(X_train, y_train,batch_size=32)
validation_set = datagen.flow(X_validation,y_validation,batch_size=32)

#fit the model to training data
classifier.fit_generator(train_set,steps_per_epoch=train_set.n,epochs=1,validation_data=validation_set,validation_steps=validation_set.n)

#predict test_data
results = classifier.predict_classes(test_data)

results = pd.DataFrame(results)
results.index.name = "ImageId"
results.index = results.index+1
results.columns = ['Label']
results.to_csv("results.csv",header=True)