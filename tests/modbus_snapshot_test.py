#!/usr/bin/env python3
"""Совместимая точка запуска теста снимка STM32 (UTF-8).

Запуск из корня: python3 tests/modbus_snapshot_test.py
Требуются Python 3, POSIX sh, cc (C99) и mktemp; на Windows — например WSL.

Вызывает tests/host/run_snapshot_test.sh: это ТОТ ЖЕ тест snapshot_test.c,
а не дополнительный независимый набор. Проверяет SNAP/STATUS/DATA, seq/ready,
заморозку снимка, микроградусы, неверные измерения и границы регистров.
Зачем: сохранить прежнюю Python-команду запуска без устаревшей второй копии
карты регистров. Полные сценарии и ограничения — в преамбуле snapshot_test.c.
Не требует платы и ничего не прошивает. Успех — код 0 (обычно без вывода);
ошибка сборки или assert дочернего теста приводит к ненулевому коду.
"""
from pathlib import Path
import subprocess

if __name__ == "__main__":
    root = Path(__file__).resolve().parents[1]
    subprocess.run(["sh", str(root / "tests/host/run_snapshot_test.sh")],
                   cwd=root, check=True)
