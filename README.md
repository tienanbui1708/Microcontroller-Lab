# Microcontroller Lab (CO3010) — HCMUT

Bùi Tiến An — 2310002

Bài lab môn Vi xử lý - Vi điều khiển: STM32F103C6, STM32CubeIDE, mô phỏng bằng Proteus 8.

| Lab | Chủ đề | Báo cáo |
|---|---|---|
| [Lab 1](Lab1) | LED Animations | [PDF](Lab1/Report/2310002_BuiTienAn_MCU_LAB1.pdf) |
| [Lab 2](Lab2) | Timer Interrupt and LED Scanning | [PDF](Lab2/Report/2310002_BuiTienAn_MCU_LAB2.pdf) |
| [Lab 3](Lab3) | Buttons/Switches | [PDF](Lab3/Report/2310002_BuiTienAn_MCU_LAB3.pdf) |
| [Lab 4](Lab4) | A Cooperative Scheduler | [PDF](Lab4/Report/2310002_BuiTienAn_MCU_LAB4.pdf) |

## Cấu trúc mỗi lab

- Project STM32CubeIDE (`.ioc`, `Core/`, `Drivers/`): import bằng *File → Import → Existing Projects into Workspace*.
- File Proteus `.pdsprj` và `.hex` đi kèm: mở bằng Proteus 8, bấm Run là mô phỏng được, không cần build lại.
- `Report/`: báo cáo PDF.

File build trong `Debug/` không được lưu (trừ `.hex` mà schematic cần), xem `.gitignore`.
Build lại trong CubeIDE sẽ tạo lại chúng.
