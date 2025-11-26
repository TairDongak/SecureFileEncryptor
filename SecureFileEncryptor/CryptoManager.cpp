#include "CryptoManager.h"
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QDebug>
#include <vector>
#include "aes.h"

// --- Вспомогательная функция для генерации ключа ---
// Она должна быть описана ДО того, как мы её вызовем в processFile
void prepareKeyAndIV(const QString& password, uint8_t* key, uint8_t* iv) {
    // Заполняем нулями для безопасности
    memset(key, 0, 32);
    memset(iv, 0, 16);

    QByteArray pwdBytes = password.toUtf8();

    // Копируем пароль в ключ (максимум 32 байта)
    int len = (pwdBytes.size() > 32) ? 32 : pwdBytes.size();
    memcpy(key, pwdBytes.data(), len);

    // Генерируем вектор инициализации (IV) на основе ключа
    // (В учебных целях делаем XOR половинок ключа)
    for (int i = 0; i < 16; i++) {
        iv[i] = key[i] ^ key[i + 16];
        if (i < pwdBytes.size()) iv[i] ^= pwdBytes[i];
    }
}

// --- Реализация Singleton (Конструктор и Деструктор) ---

// Конструктор (Приватный, согласно паттерну Singleton [cite: 14, 26])
CryptoManager::CryptoManager()
{
    qDebug() << "CryptoManager initialized (Singleton created)";
}

// Деструктор (Приватный)
CryptoManager::~CryptoManager()
{
    qDebug() << "CryptoManager destroyed";
}

// --- Основная логика ---

void CryptoManager::encryptFolder(const QString& path, const QString& password)
{
    qDebug() << "Starting encryption in folder:" << path;

    // Рекурсивный обход папок
    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        processFile(filePath, password, true); // true = шифруем
    }
}

void CryptoManager::decryptFolder(const QString& path, const QString& password)
{
    qDebug() << "Starting decryption in folder:" << path;

    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        processFile(filePath, password, false); // false = дешифруем
    }
}

void CryptoManager::processFile(const QString& filePath, const QString& password, bool encrypt)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray fileData = file.readAll();
    file.close();

    // Подготовка буферов для AES
    struct AES_ctx ctx;
    uint8_t key[32]; // 256 бит
    uint8_t iv[16];  // 128 бит

    // Теперь эта функция существует и код сработает
    prepareKeyAndIV(password, key, iv);

    // Инициализация контекста AES
    AES_init_ctx_iv(&ctx, key, iv);

    QByteArray resultData;

    if (encrypt) {
        // --- ШИФРОВАНИЕ ---

        // 1. PKCS7 Padding (дополняем данные до кратности 16)
        int paddingSize = 16 - (fileData.size() % 16);
        fileData.append(paddingSize, (char)paddingSize);

        // 2. Шифруем "на месте"
        AES_CBC_encrypt_buffer(&ctx, (uint8_t*)fileData.data(), fileData.size());

        resultData = fileData;
        qDebug() << "Encrypted:" << filePath;
    }
    else {
        // --- ДЕШИФРОВКА ---

        if (fileData.size() == 0 || fileData.size() % 16 != 0) {
            qDebug() << "Error: File size error:" << filePath;
            return;
        }

        // 1. Дешифруем "на месте"
        AES_CBC_decrypt_buffer(&ctx, (uint8_t*)fileData.data(), fileData.size());

        // 2. Удаляем Padding (проверяем последний байт)
        unsigned char lastByte = (unsigned char)fileData.at(fileData.size() - 1);

        if (lastByte > 0 && lastByte <= 16) {
            fileData.truncate(fileData.size() - lastByte);
            resultData = fileData;
            qDebug() << "Decrypted:" << filePath;
        }
        else {
            qDebug() << "Error: Invalid padding (Wrong password?)";
            return;
        }
    }

    // Запись результата
    if (file.open(QIODevice::WriteOnly)) {
        file.write(resultData);
        file.close();
    }
}