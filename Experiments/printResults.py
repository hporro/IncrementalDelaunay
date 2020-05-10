import matplotlib.pyplot as plt

numP = [1,100 ,500 ,1000 ,2000 ,3000 ,5000 ,8000 ,10000 ]
logs = [1.6e-05,0.002293 ,0.012013 ,0.033168 ,0.082571 ,0.160688 ,0.405567 ,0.85955 ,1.26048 ]
lins = [5e-06,0.001113 ,0.015911 ,0.049522 ,0.196768 ,0.445888 ,1.14325 ,2.90304 ,4.44141 ]

if __name__ == '__main__':
    plt.plot(numP,logs,label="Búsqueda de triangulos logaritmica")
    plt.plot(numP,lins,label="Búsqueda de triangulos lineal")
    plt.legend()
    plt.xlabel("Número de puntos")
    plt.ylabel("Tiempo en segundos")
    plt.title("Comparación del tiempo de construcción de la triangulación en función del número de puntos")
    plt.show()