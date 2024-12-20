# Tape sorter

### Структура

Проект разбит на 3 модуля
- converter - конвертация из текста в файл ленты и наоборот
- tapes - сортировка лент
- test - тесты 

### Сборка
``cmake -B build``

Главный модуль

``` cmake --build build --target tape-sorter```

Конвертер

``` cmake --build build --target conv```

### Тестирование
``` cmake --build build --target tests && ./build/test/test```

### Запуск 
```./conv -x tape.txt tape``` - конвертировать текстовый файл в ленту

```./conv -t tape tape.txt``` - конвертировать ленту в текстовый файл

```./tape-sorter -i <input_tape> -o <output_tape> -c config.txt```

Конфигурационный файл должен иметь следующую структуру
```
read_delay=2
write_delay=4
shift_delay=1
in_memory_limit=3
```

Где `in_memory_limit` - сколько элементов мы можем держать в памяти. Задержка в миллисекундах.

### Решение
Для парсинга конфига используется [SimpleConfigFile](https://github.com/fbarberm/SimpleConfigFile)

Для упрощения работы с файлом замапим его на память через ``mmap()``

Это сильно упростит восприятие кода и работу с `IO` (ОС сделает всё сама)

Основные недостатки данного подхода в нашем случае не существенны т.к.:

- Эмулируем крайне медленное устройство, значит кеш промахи не страшны
- Обращаемся к данным последовательно, значит можем использовать ``madvise``, чтобы подсказать ОС как лучше кешировать данные 
- Синхронизация между потоками в данной задаче не имеет значения.

Прочитать почему ``mmap`` далеко не всегда оправдан можно в [статье](https://habr.com/ru/articles/820591/).

### Алгоритм
Пусть `N` - размер ленты, `M` - максимум элементов в памяти, 
тогда нам понадобится $K = {N \over M}$ дополнительных ленточных файлов для реализации алгоритма сортировки.

1.  Загрузим `M` элементов в память, сортируем, записываем во временную ленту в обратном порядке для избежание перемоток
2.  Повторяем пункт ``1`` пока не закончится входная лента.
3.  Сливаем временные ленты в одну алгоритмом ``K-Way Merge``. Для поиска минимального элемента воспользуемся мин кучей.

### Возможные улучшения

Слияние в один проход может быть не эффективным если ``K`` слишком большое.

Можно улучшить алгоритм разделив слияние на несколько проходов, сливая `C` файлов за раз в каждом проходе, пока не останется 1.

Таким образом мы можем оптимизировать количество обращений к диску.