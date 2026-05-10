# media_scanner

Консольное приложение для Linux. Рекурсивно сканирует указанную директорию, находит медиафайлы (аудио, видео, изображения) и периодически записывает результат в файл `<path>/.media_files` в формате JSON. Опционально поднимает HTTP-сервер и отдаёт тот же JSON по `GET /media_files`.

---

## Требования

| Компонент | Минимальная версия |
|---|---|
| g++ / clang++ | C++17 |
| CMake | 3.15 |
| ОС | Linux |
| Интернет | нужен при первой сборке тестов — CMake скачает GoogleTest |

---

## Структура проекта

```
media_scanner/
├── CMakeLists.txt
├── README.md
├── include/
│   └── media_scanner/
│       ├── media_files.hpp
│       ├── config.hpp
│       ├── scanner.hpp
│       ├── json_builder.hpp
│       ├── file_writer.hpp
│       └── http_server.hpp
├── src/
│   ├── main.cpp
│   ├── scanner.cpp
│   ├── json_builder.cpp
│   ├── file_writer.cpp
│   └── http_server.cpp
└── tests/
    ├── CMakeLists.txt
    ├── test_json_builder.cpp
    ├── test_scanner.cpp
    └── test_file_writer.cpp
```

---

## Сборка

```sh
git clone <url>
cd media_scanner
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

---

## Запуск

### Базовый (домашний каталог, интервал 60 сек, без HTTP)
```sh
./build/media_scanner
```

### Указать каталог и интервал
```sh
./build/media_scanner --path /home/user --interval 30
```

### С HTTP-сервером на localhost:1234
```sh
./build/media_scanner --path /home/user --interval 30 --http
```

### Остановка
Нажать **Ctrl+C**.

---

## Проверка результата

### Файл на диске
```sh
cat ~/.media_files
```

### HTTP
```sh
./build/media_scanner --path /home/user --interval 30 --http &
curl http://localhost:1234/media_files
```

---

## Тесты

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

---

## Формат результата

```json
{
    "audio": ["111.mp3", "222.wav"],
    "video": ["333.mpg"],
    "images": ["444.jpeg", "555.png"]
}
```
