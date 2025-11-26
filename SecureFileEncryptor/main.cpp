#include <QtCore/QCoreApplication>
#include <iostream>
#include <string>
#include <QTextStream>
#include "CryptoManager.h"

// Функция для удобного ввода русского текста и строк с пробелами
QString readLine() {
    std::string line;
    std::getline(std::cin, line);
    return QString::fromStdString(line);
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    // Настройка консоли для вывода
    QTextStream cout(stdout);
    QTextStream cin(stdin);

    cout << "=== Secure File Encryptor (Singleton) ===" << Qt::endl;

    cout << "Enter folder path (full path): " << Qt::endl;
    QString path = readLine();

    cout << "Enter password: " << Qt::endl;
    QString password = readLine();

    cout << "Choose action (1 - Encrypt, 2 - Decrypt): " << Qt::endl;
    QString action = readLine();

    // --- ИСПОЛЬЗОВАНИЕ SINGLETON ---
    // Мы не пишем 'new CryptoManager()', мы просим Instance()
    if (action == "1") {
        CryptoManager::Instance().encryptFolder(path, password);
        cout << "Done!" << Qt::endl;
    }
    else if (action == "2") {
        CryptoManager::Instance().decryptFolder(path, password);
        cout << "Done!" << Qt::endl;
    }
    else {
        cout << "Unknown command." << Qt::endl;
    }

    // Для консольного приложения return 0 достаточно, чтобы выйти
    return 0;
}