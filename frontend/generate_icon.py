"""Generate app_icon.png and app_icon.ico for the SMS app."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))

from PyQt5.QtWidgets import QApplication
from PyQt5.QtGui import QPixmap, QPainter, QBrush, QColor, QFont, QLinearGradient
from PyQt5.QtCore import Qt, QRect

app = QApplication(sys.argv)

icons_path = Path(__file__).parent / "resources" / "icons"
icons_path.mkdir(parents=True, exist_ok=True)


def draw_icon(size: int) -> QPixmap:
    """Draw the SMS app icon at the given pixel size."""
    px = QPixmap(size, size)
    px.fill(Qt.transparent)
    p = QPainter(px)
    p.setRenderHint(QPainter.Antialiasing)
    p.setRenderHint(QPainter.TextAntialiasing)

    # Gradient background: deep purple → lighter purple
    grad = QLinearGradient(0, 0, size, size)
    grad.setColorAt(0.0, QColor(120, 80, 220))
    grad.setColorAt(1.0, QColor(70, 40, 160))
    p.setBrush(QBrush(grad))
    p.setPen(Qt.NoPen)
    radius = max(6, size // 6)
    p.drawRoundedRect(0, 0, size, size, radius, radius)

    # White "S" centred
    p.setPen(QColor(255, 255, 255))
    font_size = max(7, int(size * 0.48))
    font = QFont("Segoe UI", font_size, QFont.Bold)
    p.setFont(font)
    p.drawText(QRect(0, 0, size, size), Qt.AlignCenter, "S")
    p.end()
    return px


# ── Save 256px PNG for the window/taskbar icon ────────────────────
png_path = icons_path / "app_icon.png"
draw_icon(256).save(str(png_path))
print(f"Saved {png_path}  ({png_path.stat().st_size:,} bytes)")

# ── Build proper multi-size .ico with Pillow ──────────────────────
sizes = [16, 24, 32, 48, 64, 128, 256]
tmp_paths = []
for sz in sizes:
    p = icons_path / f"_tmp_{sz}.png"
    draw_icon(sz).save(str(p))
    tmp_paths.append(p)

ico_path = icons_path / "app_icon.ico"
try:
    from PIL import Image
    images = [Image.open(str(p)).convert("RGBA") for p in tmp_paths]
    images[0].save(
        str(ico_path),
        format="ICO",
        sizes=[(s, s) for s in sizes],
        append_images=images[1:],
    )
    print(f"Saved {ico_path}  ({ico_path.stat().st_size:,} bytes)  [multi-size ICO via Pillow]")
except ImportError:
    # Pillow not available — use Qt to write a 256px ICO-named PNG
    # Windows will still use it for shortcuts
    import shutil
    shutil.copy(png_path, ico_path)
    print(f"Saved {ico_path}  (PNG fallback — run: pip install Pillow  for a proper ICO)")

# Clean up temp files
for p in tmp_paths:
    p.unlink(missing_ok=True)

print("Done.")
