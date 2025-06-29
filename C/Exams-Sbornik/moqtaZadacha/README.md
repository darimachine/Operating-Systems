# Задача: **process-watcher**

Напишете програма **`process-watcher`**, която стартира и наблюдава едновременно работата на до **10** външни програми.

## Извикване:

```bash
./process-watcher prog1 prog2 … progN
```

- `N` – брой на програмите (1 ≤ N ≤ 10)
- Всяка програма се стартира **без аргументи**.

---

## Поведение на `process-watcher`

Програмата трябва да спазва следните правила за управление на процесите:

| Събитие | Действие |
|---------|----------|
| Процес приключва успешно (статус 0) | Записва се в журнала и програмата **повече не се стартира**. |
| Процес приключва неуспешно (статус ≠ 0) | Записва се в журнала и програмата **се стартира отново веднага**. |
| Процес работи над **5 секунди** | Изпраща се сигнал `SIGTERM`, записва се в журнала и програмата **повече не се стартира**. |
| Процес бъде убит от сигнал (напр. `SIGSEGV`) | Записва се в журнала, изпраща се сигнал `SIGTERM` на всички други работещи процеси и се изчаква тяхното приключване. Програмата завършва със статус `1`. |

---

## Формат на журнала

Всички събития се записват във файл с име **`myLog.log`**. Всеки ред във файла има формат:

```bash
Program <име> Started: YYYY-MM-DD HH:MM:SS Ended: YYYY-MM-DD HH:MM:SS <резултат>
```
Където `YYYY-MM-DD HH:MM:SS` е дата и час на стартиране на процеса и на завършване


Където `<резултат>` е:

- `without timeout!!` – при нормален завършек (статус 0)
- `with timeout OR was KILLED!!` – при убиване поради сигнал или превишен лимит от 5 секунди.

---

## Изисквания и ограничения:

- Всички външни програми се изпълняват едновременно (паралелно).
- Забранено е използването на `<stdio.h>`, с изключение на `snprintf()`.

- Не трябва да остават „зомби“ процеси – периодично ги събирайте с `waitpid(-1, ..., WNOHANG)`.

---  
