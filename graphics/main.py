import matplotlib.pyplot as plt
import openpyxl
from openpyxl import Workbook
import os

def read_data_from_txt(file_path):
    """
    Чтение данных из txt-файла и возврат словаря с k, T1 и T2.
    """
    data = {'k': [], 'T1': [], 'T2': []}
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines[1:]:  # Пропускаем заголовок
            parts = line.split()
            if len(parts) == 3:
                k = int(parts[0])
                T1 = float(parts[1])
                T2 = float(parts[2])
                data['k'].append(k)
                data['T1'].append(T1)
                data['T2'].append(T2)
    return data

def write_to_excel(data, output_file):
    """
    Запись данных в Excel-файл.
    """
    workbook = Workbook()
    sheet = workbook.active
    sheet.title = "Algorithm Times"

    # Заголовки
    sheet.append(["k", "T1 (Trivial Algorithm)", "T2 (KMP Algorithm)"])

    # Заполнение данных
    for i in range(len(data['k'])):
        sheet.append([data['k'][i], data['T1'][i], data['T2'][i]])

    workbook.save(output_file)
    print(f"Данные сохранены в файл {output_file}")

def plot_graphs_combined(data, file_name):
    """
    Построение графиков T1 и T2 на одном графике.
    """
    plt.figure(figsize=(10, 6))

    # Построение графиков
    plt.plot(data['k'], data['T1'], marker='o', linestyle='-', color='b', label='T1 (Trivial Algorithm)')
    plt.plot(data['k'], data['T2'], marker='o', linestyle='-', color='r', label='T2 (KMP Algorithm)')

    # Настройка графика
    plt.title(f'Сравнение времени T1 и T2 от k ({file_name})')
    plt.xlabel('k')
    plt.ylabel('Время (s)')
    plt.grid()
    plt.legend()

    # Отображение графика
    plt.tight_layout()
    plt.show()

def choose_file(directory):
    """
    Позволяет выбрать файл из указанной директории.
    """
    files = [f for f in os.listdir(directory) if f.startswith("experiment_") and f.endswith(".txt")]
    if not files:
        print("Нет файлов для выбора.")
        return None

    print("Доступные файлы:")
    for idx, file in enumerate(files, 1):
        print(f"{idx}. {file}")

    while True:
        try:
            choice = int(input("Введите номер файла для построения графика: "))
            if 1 <= choice <= len(files):
                return os.path.join(directory, files[choice - 1])
            else:
                print("Некорректный номер. Попробуйте снова.")
        except ValueError:
            print("Введите число.")

def main():
    directory = "C:\\Users\\Max\\Desktop\\aicd\\algorithms\\exp"  # Путь к директории с файлами
    output_file = "algorithm_times.xlsx"  # Имя выходного файла Excel

    # Шаг 1: Выбор файла
    file_path = choose_file(directory)
    if not file_path:
        print("Файл не выбран. Завершение программы.")
        return

    # Шаг 2: Чтение данных из выбранного файла
    data = read_data_from_txt(file_path)

    # Шаг 3: Запись данных в Excel
    write_to_excel(data, output_file)

    # Шаг 4: Построение графика
    plot_graphs_combined(data, os.path.basename(file_path))

if __name__ == "__main__":
    main()
