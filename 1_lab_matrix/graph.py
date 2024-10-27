import pandas as pd
import matplotlib.pyplot as plt


# дефолт график
# todo: сделать нормик
data = pd.read_csv('parallel.csv')
plt.plot(data['Thread'], data['Time'], marker='o')
plt.title('Время умножения матриц в зависимости от их кол-ва тредов')
plt.xlabel('Кол-во тредов')
plt.ylabel('Время (секунды)')
plt.grid(True)
plt.show()