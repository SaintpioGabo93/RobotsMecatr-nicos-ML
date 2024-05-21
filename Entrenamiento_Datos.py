import numpy as np

import tensorflow as tf

from keras.models import Sequential
from keras.layers import Dense
from keras.utils import to_categorical


from arduinoKeras import weights
from arduinoKeras import layers
from arduinoKeras import scaling

import matplotlib.pyplot as plt


# ----- Importamos el conjunto de Datos ----- #

tamanioEntrada = 1 # Este es el número de características.

# Sacamos los datos obtenidos del extractor de características

P1 = np.load('P1.npy') # Contracción meñique - pulgar
P2 = np.load('P2.npy') # Contracción anular - pulgar

P = np.concatenate((P1,P2), axis = 0)

T = np.concatenate((0*np.ones((P1.shape[0], 1)),
                    1*np.ones((P2.shape[0], 1))),
                   axis = 0)

# ------- Preparación de los datos para el algoritmo de intelitencia artificial ------ #

from sklearn.preprocessing import StandardScaler

escalador = StandardScaler().fit(P)

P = escalador.transform(P)

etiquetas_oneHot = to_categorical(T, num_classes = 2)


# ------------ Dividimos el conjunto de datos -------------------- #

from sklearn.model_selection import train_test_split

P_Entrenamiento, P_Prueba, T_Entrenamiento, T_Prueba = train_test_split(P, etiquetas_oneHot, test_size = 0.2, random_state = 42)

# ----------- Hiperparámetros ------------ #

epocas = 2000

nodosOcultos = 6

# -------- Definimos la arquitecturda de la red neuronal -------------- #

modelo = Sequential()
modelo.add(Dense(nodosOcultos, activation = 'relu', input_dim = tamanioEntrada))
modelo.add(Dense(2, activation = 'sigmoid')) # Se usa la función softmax para un problema de multiclase
modelo.summary() # Esto es para que nos muestre en pantalla el proceso

# -------- Declaración de la función de pérdida ------- #

perdida = 'categorical_crossentropy'

# ------------------ Optimizador -------------------- #

optimizador = tf.keras.optimizers.Adam(learning_rate = 0.005)

modelo.compile( loss = perdida,
                optimizer = optimizador,
                metrics = ['accuracy'])

# ----------------- Entrenamiento del modelo de la red neuronal---------------- #

history = modelo.fit(P_Entrenamiento,T_Entrenamiento,epochs = epocas, verbose = 1, validation_split = 0.1)

# ----------------- Evaluación del modelo de la red neuronal ----------------- #

prueba_perdida, prueba_exactitud = modelo.evaluate(P_Prueba,T_Prueba, verbose = 1)
print('Exactitud de la Prueba',prueba_exactitud)

# -------- Gráficas entrenamiento ------------- #

plt.xlabel('Épocas')
plt.ylabel('Pérdida')
plt.plot(history.epoch, np.array(history.history['loss']),'b', label = 'Perdida Entrenamiento') # 'b' es para blue
plt.plot(history.epoch, np.array(history.history['val_loss']),'r', label = 'Perdida Validación') # 'r' es para blue
plt.legend()
plt.grid()

# ------------- Extraemos los valores del entrenamiento ---------- #

weights(modelo.layers,3)
scaling(escalador,3)
layers(modelo.layers)

plt.show()
