def compare_files(file1_path, file2_path):
    with open(file1_path, 'r') as file1, open(file2_path, 'r') as file2:
        content1 = file1.read()
        content2 = file2.read()

        # Удаление пробелов и символов новой строки для сравнения чисел
        content1 = content1.replace(" ", "").replace("\n", "")
        content2 = content2.replace(" ", "").replace("\n", "")

        # Сравнение содержимого файлов
        if content1 == content2:
            print("Файлы содержат одинаковые числа.")
        else:
            print("Файлы содержат различные числа.")

compare_files('c_real.txt', 'c.txt')
