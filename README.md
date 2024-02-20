# Дубликатор Mifare Classic карт 

### Данный проект выполнен на основе Arduino UNO и модуля RFID RC-522 13,6мГЦ 

### Схема подключения arduino uno и модуля RC-522
![схема_подключения](https://github.com/Max-Tulskiy/Mifare-cards-cloner/assets/90907669/40527871-f068-4f30-a9cb-0f1d3af52206)

### Возможности:
+ Копирование данных карты
+ Запись UID с копии карты на новую 
+ Запись дампа с копии карты на новую
+ Проверка совпадения данных между двумя картами (копией и оригиналом).  

![скриншот](https://github.com/Max-Tulskiy/Mifare-cards-cloner/assets/90907669/da406d3d-8143-4723-b8bf-6f45b29b20e4)

### Стек:
+ C++
+ Python PyQt5
  
### Запуск локально:
+ Установка зависимостей
``` Python
pip install requirements.txt
```
+ Запуск приложения
```Python
python3 arduino_interface.py
```
