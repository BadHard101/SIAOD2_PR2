#include <iostream>
#include <string>
#include <map>
#include <ctime>
#include <Windows.h>
#include <vector>

using namespace std;

struct DataInfo {
    string address;
    vector<string>* FIO = new vector<string>();
    DataInfo(string F, string I, string O, string address) :
        address(address) {
        FIO->push_back(F);
        FIO->push_back(I);
        FIO->push_back(O);
    };
};

class DataEntry {
private:
    string phone; // номер телефона, используется как ключ
    //string address; // полезные данные
    DataInfo* info;
    bool empty = true; // индикатор того, занята ли ячейка
public:

    // создание пустой записи (empty = true)
    DataEntry() {
        info = new DataInfo("", "", "", "");
    }

    // создание записи, инициализированной значениями (empty = false)
    DataEntry(string phone, string F, string I, string O, string address) {
        info = new DataInfo(F, I, O, address);
        this->phone = phone;
        empty = false;
    }

    string getPhone() {
        return phone;
    }

    vector<string>* getFIO() {
        return info->FIO;
    }

    string getAddress() {
        return info->address;
    }

    bool isEmpty() {
        return empty;
    }
};

class DataManager {
private:
    map<string, int> hashTable; // хэш-таблица "ключ-значение"
    DataEntry* dataArray; // массив записей
    int count; // текущее количество записей

    // сумма ascii-кодов символов строки для последующего вычисления хэша
    int symbolSum(string& str) {
        int result = 0;
        for (int i = 0; i < 7; i++) {
            result += str[i];
        }
        return result;
    }

    // первый хэш: k mod N
    int hashOne(string& input) {
        return symbolSum(input) % count;
    }

    // второй хэш (для двойного хэширования): 3 - (k mod 3)
    /*int hashTwo(string& input) {
        return 3 - (symbolSum(input) % 3);
    }*/

    // метод для увеличения массива вдвое и пересчёта хэшей уже добавленных записей
    void recalculateHashes() {
        cout << "Происходит расширение массива и пересчёт хэшей." << endl;
        count *= 2; // увеличение размера массива
        DataEntry empty, temp;
        DataEntry* arrayExpanded = new DataEntry[count]; // создание нового пустого массива, вдвое большего предыдущего размера
        DataEntry* copy = dataArray; // резервная копия старого массива для сохранения доступа к элементам в процессе пересчёта хэшей
        dataArray = arrayExpanded; // замена текущего массива на пустой вдвое больший

        // цикл, проходящий по всем парам "ключ-значение" в хэш-таблице
        for (pair<string, int> keyValuePair : hashTable) {
            temp = copy[keyValuePair.second]; // сохранение записи из старого массива для повторного добавления в новый
            addEntry(temp); // добавление записи в новый пустой массив (включает в себя рассчёт хэша)
        }
        cout << "Пересчёт хэшей закончен." << endl;
    }

public:
    DataManager(int count) {
        this->count = count;
        dataArray = new DataEntry[count];
    }

    DataEntry get(string& phone) {
        return dataArray[hashTable[phone]];
    }

    int gcd(int x, int y) // взаимно простые числа (== 1 -> да, иначе -> нет)
    {
        return y ? gcd(y, x % y) : x;
    }

    void addEntry(DataEntry& entry) {
        string key = entry.getPhone();
        //int hash1 = hashOne(key), hash2 = hashTwo(key); // рассчёт двух хэшей
        int hash; 
        int c = 3; // константа для хеширования
        int d = 7; // константа для хеширования
        int i = 0; // количество шагов смещения при открытой адресации

        for (int i = 0; i < count; i++) // пересчет констант (c и d) для корректного хеширования (делаем их взаимно простыми с максимальным количеством записей)
            for (int j = 0; j < count; j++)
                if (gcd(count,gcd(i,j)) == 1)
                {
                    c = i;
                    d = j;
                    break;
                }

        while (true) {
            //hash = (hash1 + i * hash2) % count; // рассчёт хэша с учётом двойного хэширования
            hash = (hashOne(key) + c * i + d * i * i) % count;

            if (dataArray[hash].isEmpty()) { // добавление элемента в ячейку, если она не занята
                hashTable[key] = hash;
                dataArray[hash] = entry;
                break;
            }

            cout << "Произошла коллизия: " << key << " и " << dataArray[hash].getPhone() << " имеют одинаковый хэш." << endl;
            i++;
        }

        if ((float)hashTable.size() / (float)count >= 0.75) { // переход к пересчёту хэшей, если коэф. заполнения массива >= 0.75
            recalculateHashes();
        }
    }

    // удаление записи по ключу
    void removeEntry(string& phone) {
        DataEntry empty;
        dataArray[hashTable[phone]] = empty;
        hashTable.erase(phone);
    }

    // вывод всех текущих записей в массиве
    void printData() {
        cout << "Номер счета:\t\tФИО:\t\t\t\t\tАдрес:\n";
        for (pair<string, int> keyValuePair : hashTable) {
            cout << keyValuePair.first << "\t\t\t" 
                << dataArray[keyValuePair.second].getFIO()->at(0) << " "
                << dataArray[keyValuePair.second].getFIO()->at(1) << " "
                << dataArray[keyValuePair.second].getFIO()->at(2) << " "
                << "\t\t" 
                << dataArray[keyValuePair.second].getAddress() << endl;
        }
    }

};

// заполнение массива случайными записями
void addRandom(DataManager& manager, int count) {
    string cities[5]{ "Москва", "Питербург", "Ижевск", "Якутск", "Краснодар" };
    string Fs[5]{ "Кузнецов", "Васильев", "Галиханова", "Маров", "Алекперова" };
    string Is[5]{ "Андрей", "Виктор", "Эмилия", "Герман", "Алла" };
    string Os[5]{ "Александрович", "Владимирович", "Ринатовна", "Андреев", "Рауфовна" };
    for (int i = 0; i < count; i++) {
        string address = cities[rand() % 5];
        string F = Fs[rand() % 5];
        string I = Is[rand() % 5];
        string O = Os[rand() % 5];
        string phone;
        for (int j = 0; j < 7; j++) {
            phone += (char)(rand() % 7 + 48);
        }
        DataEntry entry(phone, F, I, O, address); //FIO//FIO//FIO//////////////////////////////////FIO
        manager.addEntry(entry);
    }
}

int main()
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    srand(time(NULL));
    DataManager manager(10);/////////////////
    addRandom(manager, 6);/////////////////

    cout << "Список доступных команд:" << endl << endl
        << "1. Вывод текущего состояния массива" << endl
        << "2. Добавление записи в массив" << endl
        << "3. Получение записи из массива по ключу" << endl
        << "4. Удаление записи по ключу." << endl
        << "5. Завершить работу." << endl;

    while (true) {
        cout << "Введите номер выбранной операции: ";
        int option;
        cin >> option;
        string phone;

        switch (option) {
            case 1: {
                manager.printData();
                break;
            }
            case 2: {
                string address;
                cout << "Введите счет в банке (7 символов): ";
                cin >> phone;
                phone = phone.substr(0, 7);
                string F;
                cout << "Введите Фамилию: ";
                cin >> F;
                string I;
                cout << "Введите Имя: ";
                cin >> I;
                string O;
                cout << "Введите Отчество: ";
                cin >> O;
                cout << "Введите адрес: ";
                cin >> address;
                DataEntry entry(phone, F, I, O, address);
                manager.addEntry(entry);
                cout << "Запись добавлена." << endl;
                break;
            }
            case 3: {
                cout << "Введите номер телефона нужной записи: ";
                cin >> phone;
                phone = phone.substr(0, 7);
                DataEntry entry = manager.get(phone);
                cout << entry.getFIO()->at(0) << " " << entry.getFIO()->at(1) << " " << entry.getFIO()->at(2) << '\t' << entry.getAddress() << endl;
                break;
            }
            case 4: {
                cout << "Введите номер телефона удаляемой записи: ";
                cin >> phone;
                phone = phone.substr(0, 7);
                manager.removeEntry(phone);
                cout << "Запись удалена." << endl;
                break;
            }
            case 5: {
                return 0;
            }
        }
    }
}
