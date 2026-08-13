#include <limits>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>

using namespace std;

struct Applicant {
    string fullName;
    string birthDate;
    int score;
    string faculty;
    string specialty;
    string studyForm;
    int studyPeriod;
};

const string DATA_FILE = "applicants.dat";
const string ADMIN_LOGIN = "admin";
const string ADMIN_PASSWORD = "admin123";

// получение актуальной даты
void getCurrentDate(int& year, int& month, int& day) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    year = 1900 + ltm->tm_year;
    month = 1 + ltm->tm_mon;
    day = ltm->tm_mday;
}

//парсим данные об дне рождения
void parseBirthDate(const string& birthDate, int& day, int& month, int& year) {
    sscanf(birthDate.c_str(), "%d.%d.%d", &day, &month, &year);//разбиваем полученные данные на ссылки
}

// вычисляем возраст абитуры по году рождения
int calculateAge(const string& birthDate) {
    int birthDay, birthMonth, birthYear;
    parseBirthDate(birthDate, birthDay, birthMonth, birthYear); //парсим возраст(разбиваем на нужные составляющие)

    int currentYear, currentMonth, currentDay;
    getCurrentDate(currentYear, currentMonth, currentDay);

    int age = currentYear - birthYear;// актуальный год - год рождения
    bool birthdayNotPassed = (currentMonth < birthMonth) ||
                             (currentMonth == birthMonth && currentDay < birthDay);
    if (birthdayNotPassed) {
        age--;
    }
    return age;
}

string inputString(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

//обработка выбора роли для дальнейшей работы
int inputInt(const string& prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value)) { // цикл с проверкой на корректность ввода
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); //возвращает максимальное конечное значение
        cout << "Ошибка ввода. Введите целое число: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// Записать строку в файл
void writeString(ofstream& file, const string& str) {
    size_t len = str.length();
    file.write(reinterpret_cast<const char*>(&len), sizeof(len));
    file.write(str.c_str(), len);
}

// Прочитать строку из файла
string readString(ifstream& file) {
    size_t len;
    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    char* buffer = new char[len + 1];
    file.read(buffer, len);
    buffer[len] = '\0';
    string result = buffer;
    delete[] buffer;
    return result;
}

// сохранение данных в файл dat
void saveToFile(const vector<Applicant>& applicants) {
    ofstream file(DATA_FILE, ios::binary);
    if (!file) {//если нет, либо не открывается файл dat
        cout << "Ошибка открытия файла для записи!" << endl;
        return;
    }

    size_t size = applicants.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    // а - сыллка на Applicant
    for (const auto& a : applicants) {
        writeString(file, a.fullName);
        writeString(file, a.birthDate);
        file.write(reinterpret_cast<const char*>(&a.score), sizeof(a.score));//переобразование типа данных ссылки
        writeString(file, a.faculty);
        writeString(file, a.specialty);
        writeString(file, a.studyForm);
        file.write(reinterpret_cast<const char*>(&a.studyPeriod), sizeof(a.studyPeriod));//переобразование типа данных ссылки
    }
    file.close();
}

// вызгрузка данных с dat файла
void loadFromFile(vector<Applicant>& applicants) {
    ifstream file(DATA_FILE, ios::binary);
    if (!file) {
        return;
    }

    applicants.clear();
    size_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));
    //занесение в переменные
    for (size_t i = 0; i < size; i++) {
        Applicant a;
        //парсим данные с файла
        a.fullName = readString(file);
        a.birthDate = readString(file);
        file.read(reinterpret_cast<char*>(&a.score), sizeof(a.score));
        a.faculty = readString(file);
        a.specialty = readString(file);
        a.studyForm = readString(file);
        file.read(reinterpret_cast<char*>(&a.studyPeriod), sizeof(a.studyPeriod));
        applicants.push_back(a);
    }
    file.close();
}

//функция выводящая разделитель
void printSeparator() {
    cout << "\n+--------------------------------------------------+" << endl;
}

// вывод одного абитуриента
void printApplicant(const Applicant& a, int index = -1) {
    if (index >= 0) {
        cout << "\n[" << index + 1 << "]" << endl;
    } else {
        printSeparator();
    }
    cout << "ФИО: " << a.fullName << endl;
    cout << "Дата рождения: " << a.birthDate
         << " (возраст: " << calculateAge(a.birthDate) << ")" << endl;
    cout << "Балл: " << a.score << endl;
    cout << "Факультет: " << a.faculty << endl;
    cout << "Специальность: " << a.specialty << endl;
    cout << "Форма обучения: " << a.studyForm << endl;
    cout << "Период обучения: " << a.studyPeriod << " мес." << endl;
}

// вывод всей абитуры
void showAllApplicants(const vector<Applicant>& applicants) {
    if (applicants.empty()) {
        cout << "\nСписок абитуриентов пуст!" << endl;//если в переменной нет информации о абитуре
        return;
    }

    cout << "\n+---------- СПИСОК АБИТУРИЕНТОВ ----------+" << endl;
    for (size_t i = 0; i < applicants.size(); i++) { //вывод через перебор массива данных с подсчетом количества
        printApplicant(applicants[i], i);
    }
    cout << "\n+-----------------------------------------+" << endl;
    cout << "Всего абитуриентов: " << applicants.size() << endl;
}

// функция добавления абитуриента в файл
void addApplicant(vector<Applicant>& applicants) {
    cout << "\n--- Добавление абитуриента ---" << endl;
    cin.ignore();
    Applicant a;
    a.fullName = inputString("Введите ФИО: ");
    a.birthDate = inputString("Введите дату рождения (дд.мм.гггг): ");
    a.score = inputInt("Введите балл: ");
    cin.ignore();
    a.faculty = inputString("Введите факультет: ");
    a.specialty = inputString("Введите специальность: ");
    a.studyForm = inputString("Введите форму обучения: ");
    a.studyPeriod = inputInt("Введите период обучения (месяцев): ");
    applicants.push_back(a);
    cout << "\nАбитуриент успешно добавлен!" << endl;
}

// поиск и удаление абитуриента(ов) из списка, по баллу
void deleteByScore(vector<Applicant>& applicants) {
    int minScore = inputInt("\nВведите минимальный балл: ");
    size_t initialSize = applicants.size();//сохраняем исходный размер массива

    auto it = remove_if(applicants.begin(), applicants.end(),
        [minScore](const Applicant& a) { return a.score < minScore; });//поиск абитуриентов меньше заданного балла
                                                                    //в массиве
    applicants.erase(it, applicants.end()); //удаление подходящих под условие и связанную с ними информацию

    size_t deletedCount = initialSize - applicants.size(); //подсчет удаленных абитуриентов(начальный размер - конечный размер)
    cout << "\nУдалено абитуриентов: " << deletedCount << endl;
    if (deletedCount > 0) {
        saveToFile(applicants);
        cout << "Изменения сохранены!" << endl;
    }
}

// изменение периода обучения
void changeStudyPeriod(vector<Applicant>& applicants) {
    string specialty = inputString("\nВведите специальность: "); // фильтрация основана на специальности
    int newPeriod = inputInt("Введите новый период обучения (месяцев): "); // новый период обучения

    int changedCount = 0;
    for (auto& a : applicants) {
        if (a.specialty == specialty) { //перебор по специальности
            a.studyPeriod = newPeriod; // присвоение нового периода
            changedCount++; //счет для скольких абитуриентов изменился период
        }
    }
    cout << "\nИзменено записей: " << changedCount << endl;
    if (changedCount > 0) {
        saveToFile(applicants);
        cout << "Изменения сохранены!" << endl;
    }
}

//фильтрация по возрасту
void showByAge(const vector<Applicant>& applicants) {
    int targetAge = inputInt("\nВведите возраст для поиска: ");
    bool found = false;
    cout << "\n+---------- Абитриуенты нужного возроста "
         << targetAge << " ----------+" << endl;

    for (const auto& a : applicants) {
        if (calculateAge(a.birthDate) == targetAge) { //перебор абитуриентов и сравнение их возраста
            printApplicant(a);//абитуриента(ов)
            found = true;//если было найдено, то передаем true
        }
    }
    if (!found) { // не "не найдено" то возвращаем, что с этим возрастом записей не найдено
        cout << "\nАбитуриентов с возрастом " << targetAge
             << " не найдено." << endl;
    }
}

// фильтр по факультетам
void showByFaculty(const vector<Applicant>& applicants) {
    string targetFaculty = inputString("\nВведите название факультета: ");
    bool found = false;
    cout << "\n+---------- Абитуриенты нужного факультета \""
         << targetFaculty << "\" ----------+" << endl;

    for (const auto& a : applicants) {
        if (a.faculty == targetFaculty) { //сравнение введенного фактультета с существующими в памяти
            printApplicant(a);
            found = true;
        }
    }
    if (!found) {
        cout << "\nАбитуриентов факультета \"" << targetFaculty
             << "\" не найдено." << endl;
    }
}

//меню выбора роли
int showRoleMenu() {
    cout << "\n +------ ПРИЕМНАЯ КОМИССИЯ ------+" << endl;
    cout << " |Выберите роль:" << " " << endl;
    cout << " |1. Администратор" << " " << endl;
    cout << " |2. Пользователь" << " " << endl;
    cout << " |0. Выход" << " " << endl;
    cout << " Ваш выбор:";

    int choice;
    if (!(cin >> choice)) {
        cin.clear();//очищаем строку ввода
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        choice = 0;// если было веденна буква или же число выходящее за границы(0-2), то возращаем 0
    } else {//в противном случае, возвращаем введенное число
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return choice;
}

//меню администратора
void showAdminMenu() {
    cout << "\n----------- МЕНЮ АДМИНИСТРАТОРА ------------" << endl;
    cout << "|1. Добавить абитуриента" << endl;
    cout << "|2. Показать всех абитуриентов" << endl;
    cout << "|3. Удалить по баллу (меньше указанного)" << endl;
    cout << "|4. Изменить период обучения по специальности" << endl;
    cout << "|5. Показать абитуриентов определенного возраста" << endl;
    cout << "|6. Показать абитуриентов факультета" << endl;
    cout << "|0. Выйти в главное меню" << endl;
    cout << "+-----------------------------------------------" << endl;
    cout << "Выберите действие: ";
}

//меню пользователя
void showUserMenu() {
    cout << "\n+----------- МЕНЮ ПОЛЬЗОВАТЕЛЯ -------------+" << endl;
    cout << "|1. Показать всех абитуриентов" << endl;
    cout << "|2. Показать абитуриентов определенного возраста" << endl;
    cout << "|3. Показать абитуриентов факультета" << endl;
    cout << "|0. Выйти в главное меню" << endl;
    cout << "+-----------------------------------------------" << endl;
    cout << "Выберите действие: ";
}

//авторизация под ролью администратора
bool loginAdmin() {
    string login, password;
    cout << "\nВведите логин администратора: ";
    getline(cin, login);
    cout << "Введите пароль администратора: ";
    getline(cin, password);

    if (login == ADMIN_LOGIN && password == ADMIN_PASSWORD) {
        cout << "\nВход выполнен успешно!" << endl;
        return true;
    } else {
        cout << "\nНеверный логин или пароль!" << endl;
        return false;
    }
}

//обработка меню админа
void adminMenu(vector<Applicant>& applicants) {
    int choice;
    do {
        showAdminMenu();//вызываем меню админа
        if (!(cin >> choice)) {//если число не меньше (0-2)
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1://добавление абитуриента
                addApplicant(applicants);//функция добавления абитуриента
                saveToFile(applicants);//функция сохранения информации в файл dat
                break;
            case 2:
                showAllApplicants(applicants);//вызов функции для показа информации обо всех абитуриентах
                break;
            case 3:
                deleteByScore(applicants);//удаление по баллам
                break;
            case 4:
                changeStudyPeriod(applicants);//функция для изменения периода обучения
                break;
            case 5:
                showByAge(applicants);//поиск абитуриентов опред. возроста
                break;
            case 6:
                showByFaculty(applicants);//поиск абитуриентов определенного факультета
                break;
            case 0:
                cout << "\nВыход в главное меню..." << endl;
                break;
            default:
                cout << "\nНеверный выбор!" << endl;
        }
    } while (choice != 0);
}

// Обработчик меню пользователя
void userMenu(const vector<Applicant>& applicants) {
    int choice;
    do {
        showUserMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice) {
            case 1:
                showAllApplicants(applicants);//функция показа всех абитуриентов
                break;
            case 2:
                showByAge(applicants);//фильтр по возрасту
                break;
            case 3:
                showByFaculty(applicants);//фильтр по факультету
                break;
            case 0:
                cout << "\nВыход в главное меню..." << endl;
                break;
            default:
                cout << "\nНеверный выбор!" << endl;
        }
    } while (choice != 0);//выполнять до удовлетворения условия
}

int main() {
    setlocale(LC_ALL, "Russian");
    vector<Applicant> applicants;
    loadFromFile(applicants);

    int roleChoice;
    do {
        roleChoice = showRoleMenu(); // уже считывает и очищает
        switch (roleChoice) {
            case 1:
                if (loginAdmin()) adminMenu(applicants);//логин под ролью админа
                break;
            case 2:
                cout << "\nВход как пользователь" << endl;
                userMenu(applicants);//логин под ролью пользователя
                break;
            case 0:
                cout << "\nДо свидания!" << endl;
                break;
            default:
                cout << "\nНеверный выбор!" << endl;
        }
    } while (roleChoice != 0);//выполнять до удовлетворения условия

    return 0;
}
