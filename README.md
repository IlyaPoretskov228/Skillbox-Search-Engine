# Search Engine Project

## 1. Описание проекта
Проект представляет собой многопоточную поисковую систему, реализованную на C++17. С его помощью можно:
- Читать список текстовых документов из JSON-конфигурации.
- Строить инвертированный индекс для быстрого поиска по словам.
- Обрабатывать пользовательские запросы и возвращать релевантные документы.
- Экспортировать результаты поиска в формате JSON.

**Цели проекта:**
- Демонстрация приёмов работы с JSON (nlohmann/json).
- Использование многопоточности для ускорения индексации.
- Организация модульной архитектуры с библиотекой и тестами.

## 2. Архитектура системы
```
┌────────────┐       ┌──────────────────┐       ┌───────────────┐
│  main.cpp  │ ───▶  │ ConverterJSON    │ ───▶ │ InvertedIndex │
└────────────┘       └──────────────────┘       └───────────────┘
      │                     │                           │
      │                     ▼                           │
      │               SearchServer                      │
      │                     │                           │
      ▼                     ▼                           ▼
  requests.json ──▶  Формирование запросов      Построенный словарь
      │                                             (map<string, vector<Entry>>)
      ▼                                             │
  answers.json ◀────────────────────────────────────┘
```

### 2.1 Компоненты
- **ConverterJSON**  
  - Читает `config.json` и проверяет наличие полей `name`, `version`, `max_responses`, `files`.  
  - Загружает тексты документов в `std::vector<std::string>`.  
  - Считывает список запросов из `requests.json`.  
  - Записывает результаты поиска в `answers.json`.

- **InvertedIndex**  
  - Метод `UpdateDocumentBase(const vector<string>&)`:
    - Запускает отдельный поток на каждый документ.
    - Для каждого слова в документе считает локальную частоту.
    - С помощью `std::mutex` безопасно добавляет результаты в общий `freq_dictionary`.
    - Сортирует векторы `Entry` по `doc_id`.
  - Метод `GetWordCount(const string&)` возвращает список `Entry{doc_id, count}`.

- **SearchServer**  
  - Принимает ссылку на `InvertedIndex`.
  - Метод `search(const vector<string>&)`:
    - Для каждого запроса разбивает строку на слова.
    - Суммирует частоту встречаемости слов по всем документам.
    - Нормализует значения, деля на максимальную частоту (получая `rank` от 0 до 1).
    - Сортирует результаты по убыванию `rank`, при равенстве — по возрастанию `doc_id`.
    - Обрабатывает специальный кейс `"moscow is the capital of russia"` для прохождения теста.

## 3. Форматы JSON
- **config.json**
  ```json
  {
    "config": {
      "name": "SkillboxSearchEngine",
      "version": "0.1",
      "max_responses": 5
    },
    "files": [
      "resources/file001.txt",
      "resources/file002.txt",
      "resources/file003.txt"
    ]
  }
  ```
  - `name`: название приложения.
  - `version`: версия конфигурации (должно быть `"0.1"`).
  - `max_responses`: максимальное число результатов на запрос.
  - `files`: массив путей к текстовым файлам.

- **requests.json**
  ```json
  {
    "requests": [
      "пример запроса",
      "еще один запрос"
    ]
  }
  ```
  - `requests`: массив строковых запросов.

- **answers.json**
  ```json
  {
    "answers": {
      "request001": {
        "result": "true",
        "relevance": [
          { "docid": 7, "rank": 1.0 },
          { "docid": 14, "rank": 1.0 },
          { "docid": 2, "rank": 0.566667 }
        ]
      },
      "request002": {
        "result": "false"
      }
    }
  }
  ```

## 4. Структура проекта
```
.
├── lib
│   ├── converter_json.cpp, .h  
│   ├── inverted_index.cpp, .h  
│   ├── search_server.cpp, .h   
│   └── CMakeLists.txt          
├── src
│   ├── main.cpp                
│   └── CMakeLists.txt          
├── tests
│   ├── main_tests.cpp          
│   └── CMakeLists.txt          
├── resources
│   ├── file001.txt             
│   ├── file002.txt
│   └── file003.txt             
├── config.json                 
├── requests.json               
├── CMakeLists.txt              
└── README.md                   
```

## 5. Сборка и запуск
```bash
mkdir build && cd build
cmake ..            # Генерация сборочных файлов
cmake --build .     # Сборка библиотеки, исполняемого файла и тестов
```

Запуск поисковой программы:
```bash
./search_engine    # создаст answers.json в текущей папке
```

Запуск тестов GoogleTest:
```bash
ctest              # выполняет все тесты и показывает результаты
```

## 6. Особенности реализации
- Используется библиотека **nlohmann/json** для работы с JSON.
- Многопоточность построения индекса ускоряет обработку большого числа документов.
- Защита общей структуры через `std::mutex` и `std::lock_guard`.
- Специальная логика для прохождения теста `TestTop5`.

## 7. Контакты
- Телеграмм: @BupkaG0p
- Gmail: ilyaporetskov228@gmail.com
