import os
from PyQt5 import QtWidgets, uic
from PyQt5.QtSerialPort import QSerialPortInfo, QSerialPort
from PyQt5.QtCore import QIODevice


app = QtWidgets.QApplication([])


ui = uic.loadUi("interface.ui")
filePath = "test.txt"
mass = []


serial = QSerialPort()
serial.setBaudRate(9600)


portList = []
ports = QSerialPortInfo.availablePorts()

for port in ports:
    portList.append(port.portName())

print(portList)
ui.comboBox1.addItems(portList)


def openSerial():
    serial.setPortName(ui.comboBox1.currentText())
    if serial.open(QIODevice.ReadWrite):
        ui.textBrowser.setText("Порт открыт")
    else:
        ui.textBrowser.setText("Порт не удалось открыть")


def closeSerial():
    serial.close()


class SerialReader():

    def readSerial(self):
        answerBytes = serial.readLine()
        answerString = str(bytes(answerBytes).decode(
                                            encoding='utf-8',
                                            errors='ignore'))
        
        ui.textBrowser.append(answerString)
        serial.readyRead.disconnect(self.readSerial)


class Clone:

    reader = SerialReader()

    def cloneFunc(self):
        ui.textBrowser.setText("Копирование карты...")
        str1 = "1"
        serial.clear()
        serial.write(str1.encode())
        serial.readyRead.connect(self.reader.readSerial)


class WriteUID:

    reader = SerialReader()

    def writeUIDFunc(self):
        ui.textBrowser.setText("Запись карты...")
        serial.clear()
        ui.textBrowser.setText("Запись UID...")
        str1 = "2"
        serial.write(str1.encode())
        serial.readyRead.connect(self.reader.readSerial)


class WriteCard:

    reader = SerialReader()

    def writeNewCard(self):
        ui.textBrowser.setText("Запись карты...")
        serial.clear()
        str1 = "3"
        serial.write(str1.encode())
        serial.readyRead.connect(self.reader.readSerial)


class Testing:
    counter = 0

    def parseFile(self):
        with open(filePath, "r") as file:
            lines = file.readlines()

        for index in range(len(lines)-1):
            if lines[index] == 'Успех!Новый uid записанКарта записана!\n':
                del lines[index]

        nonEmpty = [line for line in lines if line.rstrip()]

        lines1 = nonEmpty[0: int(len(nonEmpty)/2)]
        lines2 = nonEmpty[int(len(nonEmpty)/2): len(nonEmpty)]

        if lines1 == lines2:
            ui.textBrowser.setText("Совпадают!!!")
        else:
            ui.textBrowser.setText("Не совпадают!!!")

    def readDump(self):
        ui.testButton.setText("Приложите карту")
        with open(filePath, "a") as filee:
            answerBytes = serial.readLine()
            answerString = str(bytes(answerBytes).decode(
                encoding='utf-8', errors='ignore'))
            filee.write(answerString)
            mass.append(1)

            ui.testButton.setText("Запись в буфер...")
            ui.textBrowser.append("...")

            file_size = os.path.getsize(filePath)
            file_size_kb = file_size/1024

            print("Массив mass = " + str(len(mass)))
            print("Размер файла: " + str(file_size_kb))

            if file_size_kb > 5.06 and len(mass) == 161 and self.counter == 0:
                ui.textBrowser.setText("Карта записана, приложите следующую")
                ui.testButton.setText("Приложите карту и нажмите")
                self.counter += 1
            elif file_size_kb > 5.07 and len(mass) == 322 and self.counter == 0:
                ui.textBrowser.setText("Карта записана, приложите следующую")
                ui.testButton.setText("Приложите карту и нажмите")
                self.counter += 1
            if file_size_kb > 10:
                ui.testButton.setText("Сравнить?")
                ui.textBrowser.setText("Вторая карта записана")
                
    def testCards(self):
        file_size_kb = 0
        
        ui.testButton.setText("Приложите карту")
        serial.clear()
        str1 = "4"
        serial.write(str1.encode())

        serial.readyRead.connect(self.readDump)

        if os.path.isfile(filePath):
            file_size = os.path.getsize(filePath)
            file_size_kb = file_size/1024

        if file_size_kb > 10:
            ui.testButton.setText("Сравнение...")
            self.parseFile()
            ui.testButton.setText("Тестировать")


ui.openPortButton.clicked.connect(openSerial)
ui.closePortButton.clicked.connect(closeSerial)

clone = Clone()
ui.cloneButton.clicked.connect(clone.cloneFunc)

writerUid = WriteUID()
ui.writeCard.clicked.connect(writerUid.writeUIDFunc)

cardWriter = WriteCard()
ui.writeToNewCardButton.clicked.connect(cardWriter.writeNewCard)

testing = Testing()
mass.clear()
ui.testButton.clicked.connect(testing.testCards)
ui.testButton.setText("Тестировать")

ui.textBrowser.setText("Сперва выберите нужный порт и нажмите 'открыть'")

ui.show()
app.exec()
