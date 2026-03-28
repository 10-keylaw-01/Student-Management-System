"""Reusable matplotlib chart widgets for the SMS admin analytics dashboard."""

import csv
from pathlib import Path
from collections import defaultdict

import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.patches as mpatches
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import numpy as np

from PyQt5.QtWidgets import QSizePolicy
from data_paths import data_path

# ── PyOneDark palette ─────────────────────────────────────────────────────────
BG        = '#1e2127'
BG2       = '#25282f'
CARD      = '#2c313a'
TEXT      = '#c3ccdf'
TEXT2     = '#7e8a98'
ACCENT    = '#568af2'
SUCCESS   = '#98c379'
WARNING   = '#e5c07b'
ERROR     = '#e06c75'
PURPLE    = '#c678dd'
CYAN      = '#56b6c2'

CHART_COLORS = [ACCENT, SUCCESS, WARNING, ERROR, PURPLE, CYAN,
                '#e5c07b', '#61afef', '#d19a66', '#abb2bf']


def _base_fig(rows=1, cols=1, h=3.2):
    """Create a figure pre-styled with PyOneDark colours."""
    fig = Figure(figsize=(5, h), dpi=96, facecolor=CARD)
    axes = []
    for i in range(rows * cols):
        ax = fig.add_subplot(rows, cols, i + 1)
        ax.set_facecolor(BG2)
        ax.tick_params(colors=TEXT2, labelsize=8)
        for spine in ax.spines.values():
            spine.set_edgecolor(BG)
        axes.append(ax)
    # Do NOT call tight_layout here — each chart calls it once after drawing
    return fig, axes[0] if len(axes) == 1 else axes


def _canvas(fig) -> FigureCanvas:
    canvas = FigureCanvas(fig)
    canvas.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
    canvas.setStyleSheet(f"background-color: {CARD}; border-radius: 10px;")
    return canvas


# ── 1. Attendance Distribution — Pie chart ────────────────────────────────────

def make_attendance_pie(data_dir: Path = None) -> FigureCanvas:
    """Pie chart: Present vs Absent across all students."""
    present = absent = 0
    try:
        with open(data_path('attendance.csv'), 'r') as f:
            for r in csv.reader(f):
                if len(r) >= 5:
                    if r[4] == '1':
                        present += 1
                    else:
                        absent += 1
    except Exception:
        present, absent = 90, 10

    total = present + absent or 1
    sizes  = [present, absent]
    labels = [f'Present\n{present/total*100:.1f}%', f'Absent\n{absent/total*100:.1f}%']
    colors = [SUCCESS, ERROR]
    explode = (0.04, 0)

    fig, ax = _base_fig(h=3.0)
    wedges, texts = ax.pie(
        sizes, labels=None, colors=colors, explode=explode,
        startangle=90, wedgeprops=dict(width=0.6, edgecolor=CARD, linewidth=2)
    )
    ax.legend(wedges, labels, loc='lower center', ncol=2,
              frameon=False, labelcolor=TEXT, fontsize=8,
              bbox_to_anchor=(0.5, -0.08))
    ax.set_title('Attendance Distribution', color=TEXT, fontsize=10, fontweight='bold', pad=8)
    fig.tight_layout(pad=1.2)
    return _canvas(fig)


# ── 2. Fee Collection Status — Grouped bar chart ─────────────────────────────

def make_fee_bar(data_dir: Path = None) -> FigureCanvas:
    """Bar chart: paid vs outstanding per grade level."""
    paid_by_grade    = defaultdict(float)
    pending_by_grade = defaultdict(float)
    try:
        # Map student_id → grade
        sid_grade = {}
        with open(data_path('students.csv'), 'r') as f:
            for r in csv.reader(f):
                if len(r) >= 6:
                    sid_grade[r[0]] = r[5][:-1]   # e.g. '1A' → '1'
        with open(data_path('fees.csv'), 'r') as f:
            for r in csv.reader(f):
                if len(r) >= 8:
                    grade = sid_grade.get(r[1], '?')
                    paid_by_grade[grade]    += float(r[7])
                    pending_by_grade[grade] += max(0, float(r[6]) - float(r[7]))
    except Exception:
        pass

    grades = sorted(paid_by_grade.keys(), key=lambda x: int(x) if x.isdigit() else 99)
    if not grades:
        grades = [str(i) for i in range(1, 7)]
        paid_by_grade    = {g: 5000 * int(g) for g in grades}
        pending_by_grade = {g: 2000 * int(g) for g in grades}

    x   = np.arange(len(grades))
    w   = 0.55
    paid    = [paid_by_grade[g] / 1000    for g in grades]
    pending = [pending_by_grade[g] / 1000 for g in grades]

    fig, ax = _base_fig(h=3.2)
    b1 = ax.bar(x - w/2, paid,    w, color=SUCCESS, label='Paid',    zorder=3)
    b2 = ax.bar(x + w/2, pending, w, color=ERROR,   label='Pending', zorder=3)

    ax.set_xticks(x)
    ax.set_xticklabels([f'Gr {g}' for g in grades], color=TEXT2, fontsize=8)
    ax.set_ylabel('GHS (thousands)', color=TEXT2, fontsize=8)
    ax.yaxis.label.set_color(TEXT2)
    ax.tick_params(axis='y', colors=TEXT2, labelsize=8)
    ax.set_title('Fee Collection by Grade', color=TEXT, fontsize=10, fontweight='bold', pad=8)
    ax.legend(frameon=False, labelcolor=TEXT, fontsize=8)
    ax.set_axisbelow(True)
    ax.yaxis.grid(True, color=BG, linewidth=0.6)

    # Value labels on bars
    for bar in list(b1) + list(b2):
        h = bar.get_height()
        if h > 0:
            ax.text(bar.get_x() + bar.get_width() / 2, h + 0.3,
                    f'{h:.0f}k', ha='center', va='bottom', color=TEXT2, fontsize=7)
    fig.tight_layout(pad=1.4)
    return _canvas(fig)


# ── 3. Class Performance — Horizontal bar chart ───────────────────────────────

def make_class_performance_bar(data_dir: Path = None) -> FigureCanvas:
    """Horizontal bar: average grade score per class."""
    class_scores = defaultdict(list)
    try:
        # Map student_id → class
        sid_class = {}
        with open(data_path('students.csv'), 'r') as f:
            for r in csv.reader(f):
                if len(r) >= 6:
                    sid_class[r[0]] = r[5]
        with open(data_path('grades.csv'), 'r') as f:
            for r in csv.reader(f):
                if len(r) >= 5:
                    cls = sid_class.get(r[0], '')
                    if cls:
                        class_scores[cls].append(float(r[4]))
    except Exception:
        pass

    if not class_scores:
        class_scores = {f'{g}{s}': [70 + g * 2] for g in range(1, 7) for s in 'ABC'}

    classes = sorted(class_scores.keys())
    avgs    = [sum(class_scores[c]) / len(class_scores[c]) for c in classes]

    # Colour bars by performance
    bar_colors = [SUCCESS if a >= 75 else WARNING if a >= 60 else ERROR for a in avgs]

    fig, ax = _base_fig(h=max(3.2, len(classes) * 0.28))
    y = np.arange(len(classes))
    bars = ax.barh(y, avgs, color=bar_colors, height=0.72, zorder=3)

    ax.set_yticks(y)
    ax.set_yticklabels(classes, color=TEXT2, fontsize=7)
    ax.set_xlabel('Average Score (%)', color=TEXT2, fontsize=8)
    ax.xaxis.label.set_color(TEXT2)
    ax.tick_params(axis='x', colors=TEXT2, labelsize=8)
    ax.set_xlim(0, 105)
    ax.set_title('Class Average Performance', color=TEXT, fontsize=10, fontweight='bold', pad=8)
    ax.set_axisbelow(True)
    ax.xaxis.grid(True, color=BG, linewidth=0.6)

    for bar, val in zip(bars, avgs):
        ax.text(val + 0.8, bar.get_y() + bar.get_height() / 2,
                f'{val:.1f}%', va='center', color=TEXT2, fontsize=7)

    legend_patches = [
        mpatches.Patch(color=SUCCESS, label='≥ 75%'),
        mpatches.Patch(color=WARNING, label='60–74%'),
        mpatches.Patch(color=ERROR,   label='< 60%'),
    ]
    ax.legend(handles=legend_patches, frameon=False, labelcolor=TEXT,
              fontsize=7, loc='lower right')
    fig.tight_layout(pad=1.4)
    return _canvas(fig)


# ── 4. Enrollment Trend — Line chart ─────────────────────────────────────────

def make_enrollment_trend(data_dir: Path = None) -> FigureCanvas:
    """Line chart: cumulative student count per class level (1–6)."""
    grade_counts = defaultdict(int)
    try:
        with open(data_path('students.csv'), 'r') as f:
            for r in csv.reader(f):
                if len(r) >= 6:
                    grade = r[5][:-1]
                    if grade.isdigit():
                        grade_counts[int(grade)] += 1
    except Exception:
        pass

    if not grade_counts:
        grade_counts = {g: 35 + g * 3 for g in range(1, 7)}

    grades = sorted(grade_counts.keys())
    counts = [grade_counts[g] for g in grades]

    fig, ax = _base_fig(h=3.2)

    # Shaded area under line
    ax.fill_between(grades, counts, alpha=0.15, color=ACCENT)
    ax.plot(grades, counts, color=ACCENT, linewidth=2.5, marker='o',
            markersize=7, markerfacecolor=CARD, markeredgecolor=ACCENT,
            markeredgewidth=2, zorder=4)

    # Annotate each point
    for g, c in zip(grades, counts):
        ax.annotate(str(c), (g, c), textcoords='offset points',
                    xytext=(0, 9), ha='center', color=TEXT2, fontsize=8)

    ax.set_xticks(grades)
    ax.set_xticklabels([f'Grade {g}' for g in grades], color=TEXT2, fontsize=8)
    ax.set_ylabel('Students', color=TEXT2, fontsize=8)
    ax.yaxis.label.set_color(TEXT2)
    ax.tick_params(axis='y', colors=TEXT2, labelsize=8)
    ax.set_title('Enrollment by Grade Level', color=TEXT, fontsize=10, fontweight='bold', pad=8)
    ax.set_axisbelow(True)
    ax.yaxis.grid(True, color=BG, linewidth=0.6)
    ax.set_ylim(0, max(counts) * 1.25)
    fig.tight_layout(pad=1.4)
    return _canvas(fig)
