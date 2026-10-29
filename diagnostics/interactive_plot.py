#!/usr/bin/env python3
"""Интерактивный график ang_ID от seq: имя эксперимента, папка или CSV."""
import argparse
from pathlib import Path
import re
import sys
import pandas as pd
import plotly.graph_objects as go

def resolve_csv(value):
    """Experiment name, directory, or CSV; independent of working directory."""
    base = Path(__file__).resolve().parent
    text = str(value).strip().strip('"')
    if not text:
        raise ValueError('Не указано имя эксперимента или CSV')
    p = Path(text.replace('\\', '/')).expanduser()
    if p.is_absolute():
        candidates = [p]
    elif len(p.parts) == 1:
        stem = p.stem if p.suffix.lower() == '.csv' else p.name
        candidates = [base / p, base / stem / (stem + '.csv'), base / (stem + '.csv'), p]
    else:
        candidates = [p, base / p, base.parent / p]
    tried = []
    for candidate in candidates:
        if candidate.is_dir():
            candidate = candidate / (candidate.name + '.csv')
        elif not candidate.suffix:
            expected = candidate / (candidate.name + '.csv')
            if expected.is_file():
                candidate = expected
        if candidate.is_file():
            return candidate.resolve()
        tried.append(str(candidate))
    raise FileNotFoundError('CSV не найден. Проверены пути:\n  ' + '\n  '.join(dict.fromkeys(tried)))


def angle_columns(df):
    return sorted([c for c in df if re.fullmatch(r'ang_\d+', str(c), re.I)],
                  key=lambda name: int(name.split('_')[-1]))


def split_values(values):
    return [item for value in (values or []) for item in re.split(r'[,\s]+', value) if item]


def plot_angles(csv_path, html_path, auto_open=True, columns=None, x_column='seq'):
    df = pd.read_csv(csv_path, encoding='utf-8-sig', sep=None, engine='python')
    df.columns = [str(c).strip() for c in df.columns]
    if x_column not in df:
        raise ValueError(f'В CSV нет столбца {x_column}')
    columns = columns or angle_columns(df)
    if not columns:
        raise ValueError('В CSV нет столбцов ang_ID')
    missing = [c for c in columns if c not in df]
    if missing:
        raise ValueError('В CSV нет столбцов: ' + ', '.join(missing))
    x = pd.to_numeric(df[x_column], errors='coerce')
    if not x.notna().any():
        raise ValueError(f'В {x_column} нет числовых значений')
    fig = go.Figure()
    for column in columns:
        fig.add_trace(go.Scatter(x=x, y=pd.to_numeric(df[column], errors='coerce'),
                                 mode='lines', name=column, connectgaps=False,
                                 line=dict(width=1.3),
                                 hovertemplate=f'{column}: %{{y:.6f}}°<br>{x_column}: %{{x}}<extra></extra>'))
    fig.update_layout(title=f'Углы от {x_column} — {csv_path.name}', template='plotly_white',
                      height=800, xaxis=dict(title=x_column, rangeslider=dict(visible=True)),
                      yaxis=dict(title='Угол, °'), hovermode='x unified', dragmode='zoom',
                      legend=dict(orientation='h'))
    html_path = Path(html_path)
    html_path.parent.mkdir(parents=True, exist_ok=True)
    fig.write_html(str(html_path), include_plotlyjs=True, full_html=True,
                   auto_open=auto_open, config=dict(scrollZoom=True, displaylogo=False,
                                                    responsive=True, doubleClick='reset'))
    print(f'CSV: {csv_path}\nСтрок: {len(df)}\nHTML: {html_path.resolve()}')
    return fig


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('csv', nargs='?', help='Имя эксперимента, папка или CSV')
    p.add_argument('-o', '--output', type=Path)
    p.add_argument('--no-open', action='store_true')
    p.add_argument('--heads', '--ids', nargs='+', help='ID столбцов: 2,7 или 2 7; по умолчанию все')
    p.add_argument('--columns', nargs='+', help='Имена столбцов: ang_2 ang_7')
    p.add_argument('--x', default='seq')
    args = p.parse_args()
    try:
        path = resolve_csv(args.csv or input('Имя эксперимента или CSV: ').strip())
        if args.heads and args.columns:
            raise ValueError('Выберите --heads или --columns, не оба')
        columns = split_values(args.columns) or None
        if args.heads:
            ids = [int(v) for v in split_values(args.heads)]
            if any(i < 1 or i > 247 for i in ids) or len(set(ids)) != len(ids):
                raise ValueError('Нужны уникальные ID в диапазоне 1..247')
            columns = [f'ang_{i}' for i in ids]
        output = args.output or path.with_name(path.stem + '_interactive.html')
        plot_angles(path, output, not args.no_open, columns, args.x)
    except (OSError, ValueError, pd.errors.ParserError) as exc:
        print(f'[ERROR] {exc}', file=sys.stderr)
        raise SystemExit(1)

if __name__ == '__main__':
    main()
