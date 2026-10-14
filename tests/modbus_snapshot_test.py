#!/usr/bin/env python3
"""Совместимая точка запуска актуального теста SNAP / STATUS / DATA.

Используется тот же harness, что и tests/host/run_snapshot_test.sh,
без второй, устаревшей копии карты регистров и заглушек стека.
"""
from pathlib import Path
import subprocess

if __name__ == "__main__":
    root = Path(__file__).resolve().parents[1]
    subprocess.run(["sh", str(root / "tests/host/run_snapshot_test.sh")],
                   cwd=root, check=True)
