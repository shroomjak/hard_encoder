import argparse
from pathlib import Path

import pandas as pd
import plotly.graph_objects as go


def plot_angles(csv_path: Path, html_path: Path, auto_open: bool = True):
    # Автоматически определяем разделитель: запятая, точка с запятой и т. п.
    df = pd.read_csv(csv_path, sep=None, engine="python")
    df.columns = df.columns.str.strip()

    required = ["seq"]
    missing = [column for column in required if column not in df.columns]

    if missing:
        raise ValueError(
            f"В CSV отсутствуют столбцы: {', '.join(missing)}.\n"
            f"Доступные столбцы: {', '.join(df.columns)}"
        )

    for column in required:
        df[column] = pd.to_numeric(df[column], errors="coerce")

    if df["seq"].notna().sum() == 0:
        raise ValueError("В столбце seq нет числовых значений.")

    # Сохраняем исходный порядок строк и все точки.
    fig = go.Figure()

    colors = {
        "ang_1": "#1f77b4",
        "ang_2": "#ff7f0e",
    }

    for column, color in colors.items():
        if column in df.columns:
            fig.add_trace(
                go.Scatter(
                    x=df["seq"],
                    y=df[column],
                    mode="lines",
                    name=column,
                    line=dict(color=color, width=1.3),
                    connectgaps=False,
                    hovertemplate=(
                        f"{column}: %{{y:.6f}}°"
                        "<br>seq: %{x}"
                        "<extra></extra>"
                    ),
                )
            )

    fig.update_layout(
        title=dict(
            text=f"Углы от seq — {csv_path.name}",
            x=0.5,
        ),
        template="plotly_white",
        height=800,
        margin=dict(l=70, r=30, t=90, b=60),
        xaxis=dict(
            title="seq",
            showgrid=True,
            rangeslider=dict(
                visible=True,
                thickness=0.12,
            ),
            # Не блокируем масштабирование осей.
            fixedrange=False,
        ),
        yaxis=dict(
            title="Угол, °",
            showgrid=True,
            fixedrange=False,
        ),
        hovermode="x unified",
        dragmode="zoom",
        legend=dict(
            orientation="h",
            x=0,
            y=1.08,
        ),
    )

    config = {
        "scrollZoom": True,
        "displayModeBar": True,
        "displaylogo": False,
        "responsive": True,
        "doubleClick": "reset",
        "toImageButtonOptions": {
            "format": "png",
            "filename": f"{csv_path.stem}_angles",
            "scale": 2,
        },
    }

    html_path.parent.mkdir(parents=True, exist_ok=True)

    fig.write_html(
        str(html_path),
        include_plotlyjs=True,
        full_html=True,
        auto_open=auto_open,
        config=config,
    )

    print(f"Прочитано строк: {len(df)}")
    print(
        f"Диапазон seq: "
        f"{df['seq'].min():g} — {df['seq'].max():g}"
    )
    print(f"График сохранён: {html_path.resolve()}")


def main():
    parser = argparse.ArgumentParser(
        description="Интерактивный график ang_1 и ang_2 от seq."
    )
    parser.add_argument(
        "csv",
        nargs="?",
        type=Path,
        default=Path("log_test_1.csv"),
        help="Путь к CSV. По умолчанию: log_test_1.csv",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=None,
        help="Путь к итоговому HTML.",
    )
    parser.add_argument(
        "--no-open",
        action="store_true",
        help="Не открывать HTML в браузере автоматически.",
    )

    args = parser.parse_args()

    if not args.csv.is_file():
        parser.error(f"Файл не найден: {args.csv}")

    output = args.output or args.csv.with_name(
        f"{args.csv.stem}_interactive.html"
    )

    plot_angles(
        csv_path=args.csv,
        html_path=output,
        auto_open=not args.no_open,
    )


if __name__ == "__main__":
    main()