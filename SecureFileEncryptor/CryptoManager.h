#pragma once
#include <QString>
#include <QDirIterator>
#include <QDebug>

class CryptoManager
{
public:
    // Статический метод для получения единственного экземпляра (Singleton)
    // Реализует "ленивую инициализацию" [cite: 13, 21]
    static CryptoManager& Instance()
    {
        static CryptoManager instance; // Создается один раз при первом вызове
        return instance;
    }

    // Методы для выполнения задачи (бизнес-логика)
    // Пароль и путь передаем в каждый вызов, чтобы не хранить их вечно в памяти
    void encryptFolder(const QString& path, const QString& password);
    void decryptFolder(const QString& path, const QString& password);

    // УДАЛЯЕМ методы копирования и присваивания, чтобы нельзя было создать копию объекта
    // Это требование паттерна 
    CryptoManager(CryptoManager const&) = delete;
    void operator=(CryptoManager const&) = delete;

private:
    // Приватный конструктор: запрещаем создание через new [cite: 6, 26]
    CryptoManager();
    ~CryptoManager();

    // Вспомогательный метод для обработки одного файла
    void processFile(const QString& filePath, const QString& password, bool encrypt);
};