# MCU Lab 4 — Cooperative Scheduler

Đề: `../VXL_VDK_Lab_4.pdf`, mục 3–5 (trang 22–23).

## Cấu trúc

| Thư mục | Nội dung |
|---|---|
| `LAB4/` | Project CubeIDE `LAB4`, generate từ `LAB4/LAB4.ioc` (Core, Drivers, .project nằm ngay đây) |
| `LAB4/user_code/` | `scheduler.c/.h` đã viết sẵn, chép vào `LAB4/Core` sau khi generate |
| `LAB4/build.ps1` | Build headless bằng CubeIDE rồi chép HEX sang `Proteus/lab_4.hex` |
| `LAB4/sync_hex.ps1` | Dùng khi build bằng nút trong CubeIDE: chỉ chép HEX |
| `Proteus/lab4.pdsprj` | Bản sao schematic Lab 4 của Thinh, nạp `lab_4.hex` cùng thư mục |
| `Reference_Thinh/` | Source gốc `lab_4` + schematic của Thinh (chỉ để đọc, chỉ có ở máy, không đưa lên GitHub; bản gốc: github.com/thinhnguyenhuu50/MCU_CO3010_LAB) |
| `tests/` | Host test cho scheduler (gcc, HAL giả) |
| `docs/` | Ghi chú, kịch bản demo |

Báo cáo: folder riêng `E:\Code\Latex Report\Microcontroller\Lab4`. Sửa nội dung trong
`source/content/bai_4.tex`, build `main.tex` bằng pdflatex (chạy 2 lần cho mục lục).
`source/code` là bản chép của `scheduler.c/.h` và phần USER CODE của `main.c` — sửa code thì chép lại.
GitHub `tienanbui1708/Microcontroller-Lab` đang lưu mỗi lab thành một file zip (`MCU_LAB3.zip`, ...).

## Yêu cầu đề bài (tóm tắt)

- `void SCH_Update(void)` — gọi trong ngắt timer, tick **10 ms**.
- `void SCH_Dispatch_Tasks(void)` — chạy task đến hạn trong vòng lặp chính.
- `uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD)` — trả về **ID**.
- `uint8_t SCH_Delete_Task(uint32_t taskID)` — xóa theo **ID**.
- `main` có **5 task định kỳ**: 0.5 s, 1 s, 1.5 s, 2 s, 2.5 s.
- Demo: nhiều timeout cùng lúc, đúng thứ tự, in timestamp (`get_time`) chính xác tới 10 ms,
  vài one-shot thêm trước vòng lặp chính.
- Không đạt nếu: chỉ một timeout, callback sai thứ tự, **tìm kiếm O(n) trong `SCH_Update`**.

## Bước 1 — CubeMX (tự làm)

Đã làm: `LAB4/LAB4.ioc`, Project Name **`LAB4`**, Toolchain **STM32CubeIDE**.
HEX build ra là `LAB4/Debug/LAB4.hex`; script chép thành `Proteus/lab_4.hex` vì schematic nạp tên đó.

- MCU **STM32F103C6Tx** (Lab 3 dùng C6Ux; cùng chip, chỉ khác vỏ, firmware như nhau), clock **HSI 8 MHz**, không PLL (SYSCLK = APB1 = APB2 = 8 MHz).
- SYS → Debug: **No Debug** (như Lab 3, giải phóng PB3/PB4).
- TIM2: Clock Source Internal, **Prescaler 79, Period 999** → 8 MHz / 80 / 1000 = 100 Hz = **10 ms**.
  (Thinh dùng 799/9 = 1 ms — sai yêu cầu 10 ms của đề.)
  Không dùng 7999/9: đo trong Proteus thì tick chỉ 9 ms (firmware in 5500 ms khi Proteus
  dừng ở 5.05 s; 4 lần đo đều khớp tỉ lệ 10/9) — mô hình TIM2 của Proteus chia cho ARR
  thay vì ARR+1. ARR = 999 làm sai lệch đó còn ~0.1%; trên chip thật hai cấu hình như nhau.
- NVIC: bật **TIM2 global interrupt**.
- Project Manager → Code Generator: bật *Generate peripheral initialization as a pair of .c/.h*.
- GPIO output: push-pull, mức khởi tạo **HIGH** (LED tích cực mức thấp như Lab 3).

- USART1: Asynchronous, **115200** 8N1 (mặc định CubeMX), NVIC USART1 bật.
  Không dùng 9600: in timestamp mỗi 10 ms sẽ nghẽn.

Pinout — **phương án A**: giữ board Lab 3/Thinh, dời EN_1/EN_2 để lấy PA9/PA10 cho UART.
Lab 6 sẽ vẽ lại schematic theo bảng pin của đề Lab 6.

| Chức năng | Pin | Label | So với Thinh |
|---|---|---|---|
| LED debug | PA0 | `LED_DEBUG` | giữ |
| Segment a–g | PA1–PA7 | `LED7SEG_a` … `LED7SEG_g` | giữ |
| Enable digit 0 / 3 | PA8 / PA11 | `EN_0` / `EN_3` | giữ |
| Enable digit 1 / 2 | **PA12 / PA15** | `EN_1` / `EN_2` | **dời từ PA9 / PA10** |
| UART TX / RX | **PA9 / PA10** | USART1 | **mới** |
| Nút mode / tăng / set | PB0 / PB1 / PB2 (input, no pull) | `BUTTON_0..2` | giữ |
| Đường 1 xanh / vàng / đỏ | PB3 / PB4 / PB5 | `GREEN1` `AMBER1` `RED1` | giữ |
| Đường 2 xanh / vàng / đỏ | PB6 / PB7 / PB8 | `GREEN2` `AMBER2` `RED2` | giữ |
| Task 0.5 / 1 / 1.5 / 2 / 2.5 s | **PB9–PB13** | `LED_T1` … `LED_T5` | **mới** |

PA15 mặc định là JTDI: bắt buộc SYS = No Debug. Còn trống: PB14, PB15, PC13–PC15.

Schematic `Proteus/lab4.pdsprj` cần sửa theo bảng: nối lại EN_1 → PA12, EN_2 → PA15;
thêm 5 LED (tích cực mức thấp) vào PB9–PB13; thêm Virtual Terminal, RXD ← PA9, TXD → PA10, 115200.

## Bước 2 — Sau khi generate

1. Chép `LAB4/user_code/Inc/scheduler.h` → `LAB4/Core/Inc/`,
   `LAB4/user_code/Src/scheduler.c` → `LAB4/Core/Src/`.
2. Trong `main.c` (chỉ viết trong vùng `USER CODE`):

```c
/* USER CODE BEGIN Includes */
#include "scheduler.h"
/* USER CODE END Includes */

/* USER CODE BEGIN 2 */
SCH_Init();
HAL_TIM_Base_Start_IT(&htim2);
SCH_Add_Task(task_0_5s, 500, 500);
/* ... 4 task còn lại ... */
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
SCH_Dispatch_Tasks();
/* USER CODE END 3 */

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		SCH_Update();
	}
}
/* USER CODE END 4 */
```

3. CubeIDE: Project → Properties → C/C++ Build → Settings → MCU Post build outputs →
   bật **Convert to Intel Hex file**.

## Bước 3 — Build và mô phỏng

```powershell
powershell -ExecutionPolicy Bypass -File .\LAB4\build.ps1     # build + chép HEX
powershell -ExecutionPolicy Bypass -File .\LAB4\sync_hex.ps1  # nếu build bằng nút trong CubeIDE
powershell -ExecutionPolicy Bypass -File .\tests\run.ps1      # host test scheduler
```

Mở `Proteus/lab4.pdsprj`. Program File của U1 đang trỏ thẳng `..\LAB4\Debug\LAB4.hex`,
nên build trong CubeIDE là đủ, không cần sync (bản `Proteus/lab_4.hex` chỉ để dự phòng).
Sau mỗi lần build: **Stop rồi Run**.
Lab 3 có lúc phải mở Proteus bằng quyền Administrator thì nút mới nhận.

## Scheduler đã viết (`LAB4/user_code`)

- Delta queue: danh sách liên kết trên mảng tĩnh, sắp theo thời điểm đến hạn; mỗi phần tử
  lưu số tick sau phần tử đứng trước. `SCH_Update` chỉ giảm phần tử đầu → **O(1)**.
- Task cùng thời điểm chạy theo thứ tự gọi `SCH_Add_Task` (mỗi task có số thứ tự `seq`),
  kể cả task định kỳ sau khi được đưa lại vào queue: ở 20000 ms luôn là T1, T2, T4, T5.
  (Bản đầu chỉ xếp sau các task cùng hạn nên thứ tự bị đảo — thấy trong Proteus.)
- ID = `(generation << 8) | slot`: ID cũ không xóa nhầm task mới dùng lại slot.
- Tick đến khi task đầu đã tới hạn mà main chưa kịp chạy được cộng dồn (`late_ticks`)
  và trừ cho các task phía sau → không mất thời gian khi một task chạy lâu.
- Dispatcher thao tác danh sách trong vùng cấm ngắt, gọi task khi ngắt đã bật lại.
  `__WFI()` chạy khi ngắt đã bật: mô hình CM3 của Proteus không thức dậy từ WFI khi
  PRIMASK = 1 (bản đầu bị treo vì vậy). Tick rơi ngay trước WFI được xử lý ở SysTick
  kế tiếp (1 ms), nên task trễ tối đa < 1 ms.
- `SCH_Get_Time()` trả ms từ lúc `SCH_Init`, độ phân giải 10 ms.

Host test (`tests/`) đã pass: 5 task trong 15 s chạy đúng số lần và đúng bội 500 ms;
FIFO; one-shot; xóa và ID cũ; tick trễ; queue đầy; xóa task đầu khi đang đếm.
Đây là test logic trên PC, chưa phải kết quả chạy trong Proteus.

## Demo trong `main.c` (mục 5 của đề)

| Yêu cầu | Cách làm |
|---|---|
| 5 task 0.5–2.5 s | `task_0_5s` … `task_2_5s`, mỗi task đảo LED_T1..T5 và in timestamp |
| Timeout 10 ms in `get_time` | `task_10ms` in ở 10, 20, …, 100 ms |
| Xóa task theo ID | `stop_10ms` (one-shot 100 ms) xóa `task_10ms` → 1, xóa lại ID cũ → 0 |
| One-shot trước main loop | 250, 1250, 3330 ms, in thời điểm chạy |
| Đúng thứ tự | Task trùng mốc chạy theo thứ tự đăng ký (T1 trước T2 …) |

Terminal mong đợi (đã chạy cùng chuỗi đăng ký trên PC):

```
[       0 ms] Lab 4 scheduler demo start
[      10 ms] tick 10ms
...
[     100 ms] tick 10ms
[     100 ms] delete ID 0x0105 -> 1, again -> 0
[     250 ms] one-shot 250 ms
[     500 ms] T1 0.5s
[    1000 ms] T1 0.5s
[    1000 ms] T2 1s
[    1250 ms] one-shot 1250 ms
...
[    3330 ms] one-shot 3330 ms
```

## Ghi chú về Lab 4 của Thinh (`Reference_Thinh`)

Thinh dùng scheduler để chạy lại bài đèn giao thông Lab 3 (`fsm_auto`, `fsm_config`).
Các điểm lệch so với đề, không nên chép nguyên:

- API khác tên và kiểu: `SCH_add` không trả ID, `SCH_delete` xóa theo con trỏ hàm
  (đề yêu cầu `SCH_Add_Task` trả ID và `SCH_Delete_Task(ID)`).
- Tick 1 ms (PSC 799) thay vì 10 ms.
- Không có 5 task 0.5–2.5 s, không có in timestamp.
- `SCH_update` chỉ đánh dấu task đầu; task phía sau có delay 0 phải chờ thêm 1 tick → trễ dần.
- Dispatcher sửa mảng trong main mà không chặn ngắt, trong khi ISR cũng sửa phần tử đầu.
- `SCH_add`: khi chèn ở cuối mảng vẫn ghi vào `SCH_tasks_G[idx + 1]` → tràn mảng nếu queue gần đầy.
- `button.c` dùng chung một `button_flag` cho 3 nút (cùng lỗi đã sửa ở Lab 3).

Giữ được: schematic, pinout, driver LED 7 đoạn, ý tưởng chạy FSM như các task 10 ms.
