<div align="center">

# FreeRTOS Benchmark

</div>

## Configuration

- FreeRTOS Version: 9.0.0.
- Chips include: **STM32F767IG** and **CH32V307VC**.
- Optimization level: -O3 or -Of.
- More specific implementation details can be seen from source code.

## Results

<div align="center">

![STM32F767IG](./Figures/STM32F767.png)

</div>

So far, the test result is as follows.

|     Chip     | Yield | Notify | Semphore | Queue | Memory | ISR Notify | ISR Semphore | ISR Queue |
| :----------: | :---: | :----: | :------: | :---: | :----: | :--------: | :----------: | :-------: |
|  STM32F767   |  176  |  364   |   500    |  579  |  295   |    376     |     480      |    581    |
|  CH32V307    |  TBD  |  TBD   |   TBD    |  TBD  |  TBD   |    TBD     |     TBD      |    TBD    |