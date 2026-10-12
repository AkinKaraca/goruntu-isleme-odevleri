from pathlib import Path

import numpy as np
from PIL import Image, ImageFilter


folder = Path(__file__).resolve().parent
image = Image.open(folder / "duvar.jpeg").convert("L")
smoothed = image.filter(ImageFilter.GaussianBlur(radius=1.0))
pixels = np.asarray(smoothed, dtype=np.float32)
padded = np.pad(pixels, ((1, 1), (1, 1)), mode="edge")
kernel = np.array(
    [[-1, 0, 1], [-1, 0, 1], [-1, 0, 1]],
    dtype=np.float32,
)
response = np.zeros_like(pixels)
for row in range(3):
    for column in range(3):
        response += kernel[row, column] * padded[
            row : row + pixels.shape[0], column : column + pixels.shape[1]
        ]
edges = np.abs(response)
maximum = edges.max()
if maximum > 0:
    edges = edges * (255.0 / maximum)
edge_image = Image.fromarray(np.clip(edges, 0, 255).astype(np.uint8))

image.save(folder / "gri_ton.png")
smoothed.save(folder / "gauss_yumusatma.png")
edge_image.save(folder / "prewitt_sonuc.png")
