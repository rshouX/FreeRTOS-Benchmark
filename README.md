# FreeRTOS Benchmark on STM32F767

So far, the test result is as follows.

| Param | Yield | Notify | Semphore | Queue | Memory | ISR Notify | ISR Semphore | ISR Queue |
| :---: | :---: | :----: | :------: | :---: | :----: | :--------: | :----------: | :-------: |
|  AVG  |  176  |  364   |   500    |  579  |  295   |    376     |     480      |    581    |
|  MAX  |  248  |  480   |   676    |  696  |  339   |    516     |     600      |    692    |
|  MIN  |  176  |  364   |   500    |  576  |  250   |    581     |     692      |    576    |