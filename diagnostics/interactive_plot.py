#!/usr/bin/env python3
"""Интерактивные графики выбранных ang_<ID> от seq.

python diagnostics/interactive_plot.py run_01 --ids 2,7,12
HTML сохраняется в diagnostics/run_01/run_01_interactive.html.
"""
from __future__ import annotations

import argparse
from pathlib import Path
import sys

import pandas as pd
import plotly.graph_objects as go

DIAGNOSTICS = Path(__file__).resolve().parent


def parse_ids(text: str) -> tuple[int, ...]:
    try:
        ids = tuple(int(p) for p in text.split(','))
        if not ids or len(set(ids)) != len(ids) or any(not 1 <= i <= 247 for i in ids):
            raise ValueError
        return ids
    except ValueError:
        raise argparse.ArgumentTypeError('Укажите уникальные ID 1..247: 2,7,12') from None


def experiment_csv(name: str) -> Path:
    if not name or '/' in name or '\\' in name or name in ('.', '..'):
        raise ValueError('Укажите имя папки внутри diagnostics')
    return DIAGNOSTICS / name / f'{name}.csv'


def plot_angles(csv_path: Path, html_path: Path, ids: tuple[int, ...], auto_open: bool = True):
    columns = [f'ang_{head}' for head in ids]
    df = pd.read_csv(csv_path, encoding='utf-8-sig')
    df.columns = df.columns.str.strip()
    df = df[['seq', *columns]].apply(pd.to_numeric, errors='coerce')
    fig = go.Figure()
    for column in columns:
        fig.add_trace(go.Scattergl(
            x=df['seq'], y=df[column], mode='lines', name=column,
            line=dict(width=1.3), connectgaps=False,
            hovertemplate=f'{column}: %{{y:.6f}}°<br>seq: %{{x}}<extra></extra>',
        ))
    fig.update_layout(
        title=dict(text=f'Углы от seq — {csv_path.name}', x=0.5),
        template='plotly_white', height=800,
        margin=dict(l=70, r=30, t=90, b=60),
        xaxis=dict(title='seq', showgrid=True, fixedrange=False,
                   rangeslider=dict(visible=True, thickness=0.12)),
        yaxis=dict(title='Угол, °', showgrid=True, fixedrange=False),
        hovermode='x unified', dragmode='zoom',
        legend=dict(orientation='h', x=0, y=1.08),
    )
    fig.write_html(
        str(html_path), include_plotlyjs=True, full_html=True, auto_open=auto_open,
        config=dict(scrollZoom=True, displayModeBar=True, displaylogo=False,
                    responsive=True, doubleClick='reset',
                    toImageButtonOptions=dict(format='png', filename=f'{csv_path.stem}_angles', scale=2)),
    )
    return fig


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('folder', help='Имя папки эксперимента внутри diagnostics')
    parser.add_argument('--ids', required=True, type=parse_ids, help='Головки для графика: 2,7,12')
    parser.add_argument('--no-open', action='store_true', help='Не открывать браузер')
    args = parser.parse_args()
    try:
        path = experiment_csv(args.folder)
        plot_angles(path, path.with_name(f'{path.stem}_interactive.html'),
                    args.ids, auto_open=not args.no_open)
    except (OSError, ValueError, KeyError, pd.errors.ParserError) as exc:
        print(f'Error: {exc}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
