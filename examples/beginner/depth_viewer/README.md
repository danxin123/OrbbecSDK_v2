# Depth Viewer

View depth stream with **3D rendering**, **raw depth inspection**, and **PNG save**.

## Controls

| Key | Action |
|-----|--------|
| **M** | Toggle 3D depth rendering (Scharr gradient lighting) |
| **S** | Save current depth as 16-bit PNG |
| **D** | Toggle raw depth normalized display |
| **C** | Cycle colormap (JET → TURBO → MAGMA → INFERNO → PLASMA) |
| **Esc** | Exit |

## Key Concepts

- **`DepthFrame::getValueScale()`** — Converts raw uint16 → millimeters
- **`renderDepth3D()`** — Applies Scharr gradient + colormap for 3D-like visualization
- **`cv::Mat(h, w, CV_16UC1, getData())`** — Direct access to raw 16-bit depth buffer

## Next Steps

- [rgbd_viewer](../rgbd_viewer/) — Add color stream and depth-to-color alignment
- [3d_measurement](../../application/3d_measurement/) — Measure real-world distances
