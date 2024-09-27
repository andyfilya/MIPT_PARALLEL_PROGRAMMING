import pandas as pd
import matplotlib.pyplot as plt


# дефолт график
# todo: сделать нормик
data = pd.read_csv('times.csv')
plt.plot(data['Size'], data['Time'], marker='o')
plt.title('Время умножения матриц в зависимости от их размера')
plt.xlabel('Размер матрицы (NxN)')
plt.ylabel('Время (секунды)')
plt.grid(True)
plt.show()