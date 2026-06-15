# План разработки BLDC FOC контроллера K1946VK035

## Текущее состояние
- **Firmware:** Базовый PWM demo (sinusoidal open-loop, 3-phase, no FOC)
- **Build:** Eclipse/Vector IDE generated makefile (`K1921VK035_POWER_BOARD/Debug/`)
- **MCU:** K1946VK035 (Cortex-M4F, FPU present, 64K Flash, 16K RAM)
- **Схема:** `resources/PLAN.pdf` — full power stage (3-phase MOSFET bridge, shunt current sensing, gate drivers, protection, CAN/UART)

---

## Фаза 1: Аппаратная авария по сверхтоку (PWM_TZ) — ТЕКУЩАЯ ЗАДАЧА

### Описание
Внешний сигнал с компараторов тока → вход PWM_TZ (Руководство пользователя 14.8 Детектор сигнала аварии).
- Цифровой фильтр 200-500нс (регистр FWDTH)
- Асинхронный сброс выходов всех ШИМ блоков в лог. 0
- Прерывание по событию → переход автомата состояний в «аппаратная авария по сверхтоку»
- Индикация: LED2 (PA15) мигание

### Файлы для изменения
| Файл | Действие |
|------|----------|
| `include/PWM_Control.h` | Добавить `PWM_TZ_Protection_Init()`, `fsm_state_t`, extern `g_foc_state` |
| `src/PWM_Control.c` | Добавить `PWM_TZ_Protection_Init()` |
| `src/main.c` | FSM, ISR handlers, LED fault blinking, вызов init |

### Реализация

#### PWM_TZ_Protection_Init()
- **TZSEL**: `TZSEL_bit.OSHT = TZ_ENABLE` — One-Shot trip по TZ0 (внешний сигнал компаратора)
- **TZCTL**: `TZCTL_bit.TZA = TZ_FORCE_LO`, `TZCTL_bit.TZB = TZ_FORCE_LO` — асинхронный сброс всех выходов в 0
- **FWDTH**: `= 15` — ~300нс фильтр при ~50MHz PWM clock (расчёт: 20нс/такт × 15 = 300нс)
- **TZEINT**: `TZEINT_bit.OST = 1` — enable One-Shot TZ interrupt
- **TZINTCLR**: clear pending interrupt
- **NVIC**: Enable IRQ для PWM0_TZ (IRQn 42), PWM1_TZ (IRQn 45), PWM2_TZ (IRQn 48)
- Вызвать для всех 3 PWM модулей

#### ISR (override weak handlers из startup_K1921VK035.S)
```c
void PWM0_TZ_IRQHandler(void) {
    g_foc_state = STATE_OVERCURRENT_FAULT;
    PWM0->TZINTCLR_bit.INT = 1;
    NVIC_ClearPendingIRQ(PWM0_TZ_IRQn);
}
// Аналогично PWM1_TZ_IRQHandler, PWM2_TZ_IRQHandler
```

#### Main loop — индикация LED2 мигание при аварии
```c
if (g_foc_state == STATE_OVERCURRENT_FAULT) {
    fault_led_timer++;
    if (fault_led_timer > 500000) {
        fault_led_timer = 0;
        GPIOA->DATA ^= (1 << 15);  // Toggle LED2
    }
    continue;  // Не обновляем CMPA
}
```

### Вопросы перед реализацией
1. **TZ0 pin** — какой конкретно TZ source используется (TZ0/TZ1/TZ2)?
2. **One-Shot vs Cycle-by-Cycle** — для сверхтока One-Shot (блокирует до программного сброса) — подтвердить?
3. **LED2 = PA15** — в текущем коде PA15 это BUTTON_PIN (вход). На схеме LED2 — это PA15 или другой pin?

---

## Фаза 2: Инфраструктура и базовая инициализация
1. **Перенос на CMake** — уйти от Eclipse makefile, создать `CMakeLists.txt` с arm-none-eabi toolchain
2. **Исправить линкер** — `cmd/K1921VK035_flash.ld`: убрать Windows пути, настроить секции `.fastcode` в RAM для ISR
3. **System init** — корректная настройка PLL, тактирования периферии (RCU, GPIO, PWM, ADC)
4. **GPIO/PWM init** — модульная структура, dead-time verification по схеме
5. **Startup framework** — определить состояние ротора (locked/free), parameter ID stub

## Фаза 3: ADC и токовая измерительная цепь
6. **ADC инициализация** — синхронизация с PWM (center-aligned trigger), настройка sequencer для фазных токов
7. **Shunt current sensing** — калибровка offset, фильтрация (LPF), преобразование в Q15
8. **ADC ISR** — минимальный: чтение результатов, сохранение в buffer, flag для FOC loop
9. **Overcurrent protection** — hardware comparator + TZ forced shutdown (перекрывается с Фазой 1)

## Фаза 4: FOC алгоритм
10. **Clarke transform** — Ia, Ib → Iα, Iβ (Q15)
11. **Park transform** — Iα, Iβ → Id, Iq (нужен угол ротора)
12. **PI controllers** — Id loop, Iq loop (anti-windup, Q15 coefficients)
13. **Inverse Park + SVPWM** — Vd, Vq → Va, Vb, Vc → duty cycles для PWM CMPA
14. **Speed/position estimation** — observer/PLL на back-EMF (если нет энкодера)

## Фаза 5: Управление и защита
15. **Speed controller** — внешний PI loop → reference Iq
16. **Fault handling** — overcurrent, overvoltage, undervoltage, desat → все фазы OFF
17. **State machine** — IDLE → ALIGN → OPEN_LOOP → CLOSED_LOOP → FAULT
18. **Watchdog** — serviced в main loop, не в ISR

## Фаза 6: Коммуникация и отладка
19. **CAN/UART** — команды скорости, параметры, telemetry
20. **DWT profiling** — измерение времени выполнения FOC ISR
21. **Parameter tuning** — PI gains, current loop bandwidth, speed loop

---

## Приоритетный порядок
```
1 → 2 → 3 → 4 → 5 → 6 → 7 → 8 → 9 → 10 → 11 → 12 → 13 → 16 → 17 → 14 → 15 → 18 → 19 → 20 → 21
```

## Ключевые зависимости
- ADC sync с PWM (п.6) блокирует весь FOC
- Current sensing (п.7) — без него нет FOC
- SVPWM (п.13) — нужен для корректного управления инвертором по схеме
- Fault handling (п.16) — обязательно до закрытого контура

## Справочная информация
- **SDK:** `../k1921vkx_sdk/` — official NIIET SDK
- **Datasheet:** `resources/RP.pdf`
- **Схема:** `resources/PLAN.pdf`
- **Соседние репозитории:** `../esc-firmware/` (scalar BLDC), `../dependencies/` (toolchains)
- **Toolchain:** arm-none-eabi-gcc, `-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16`
- **Memory:** 64K Flash @ 0x0, 16K RAM @ 0x20000000, Heap=0, Stack=3KB
