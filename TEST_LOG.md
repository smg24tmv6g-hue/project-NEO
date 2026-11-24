## 2026-11-05 USART1 echo (polling) 
- 方式：宿主机编译+审查+上板实验
- 配置：PA9(TX)/PA10(RX)，115200 8N1
- 结果：构建绿 FLASH 25048B；回声逻辑已上板验证
- 欠账：上板环回、ORE burst 实验、配对文件开关决策（Ch3 再定）
- 证据：115200波特率下能跑约230个字节 RDR容纳一个字节
## 2026-11-05 Button (EXTI)
- 方式：宿主机编译+审查+上板实验
- 配置：EXTI 下降沿 + NVIC(EXTI15_10)
- 结果：构建绿 FLASH  25328B；按键逻辑已定
- 欠账：(消抖阻塞期回声 ORE、无超时等待循环故障模式，无上板验证)销毁 换成EXTI中断
- 证据：阈值理由：按下时 到最后几毫秒才是真正起作用的沿，设定100ms足够完成沿的判断 
- 合并语义：按下两次loop只会消费一次flag 第二次判定为空操作之后不再发送字符 两次并为一次 
- sub-tick：同一极短时间的两个沿会被吞掉一个 不会产生误判
## 2026-11-7 WS2812
- 方式：宿主机编译+审查+单测四向量绿（test_ws2812.c, gcc @ D:\MSYS2\ucrt64+上板实验
- 配置：PA7/SPI6/6.0MHz/CPHA
- 结果：构建绿 FLASH  28156B；颜色变化已上板验证
- 欠账：latch 2ms阻塞（115200 下≈23 字节盲窗）
- 证据：1EDGE（例程配置） 0xC0、0xF0来源于ws2812datasheet  RES 280µs（11-13将2EDGE改为1EDGE 复验确认）
## 2026-11-7 Buzzer
- 方式：宿主机编译+审查+上板实验
- 配置：PB15 TIM12_CH2 + 无源 4000Hz 
- 结果：构建绿 FLASH   28948B；已上板验证
- 证据：最大4kHz（手册）4kHz=4mHz/（999+1）2kHz=4mHz/（1999+1）
- 4kHz占空比70%（最大时是直流电 不会响） 2kHz占空比50% CRR=0时静音 触发时按键换音 无阻塞鸣响
- 说明：PSC固定 换音调只改ARR
## 2026-11-10 Checking
- 回声：上板通过：hello 环回正确；发123回123
- 按键：上板通过：按一次就响一下 两种音调换着响
- WS2812：上板通过：红绿蓝橙轮转顺序正确、无红绿互换
- 蜂鸣器：上板通过：4k/2k双音调分辨、200ms自停
## 2026-11-8 App层构建
- 操作：将ws2812_encode.h/.c移入App test_ws2812.c放入App\test
- 方式：cd D:\firsttry\projectneo
 gcc -Wall -Wextra App/ws2812_encode.c App/test_ws2812.c -o test_ws2812.exe
.\test_ws2812.exe 宿主机构建
## 2026-11-12 Ch3
- App/ 分离：152fa21；status 无 App/、构建绿、单测重跑绿
- 再生成演练：e9eabd7；生成区四文件/App 零行/构建绿 28948B
- PA5 速度档：52b0adf（6MHz 边沿质量）
- 教训：盲 reset 事故一次——指针/磁盘/远程三地方模型入册；CubeMX 只碰干净树
## 2026-11-14 bsp_led and bsp_buzzer
- 前瞻：两者均放入BSP层 bsp_led入LED/ bsp_buzzer入BUZZER/
- 结果：构建绿 已commit+push main.c得到瘦身效果 bsp函数可以直接调用
- 备注：初始化状态绿灯闪两次和 循环状态蓝/青 250ms/250ms切换着亮（11-24由于rtos蓝灯亮灭过程有噪音加入青色） 由于暂无error 红色指示灯不编入
- 欠账：ERROR 态未实现（无可观测故障源）
## 2026-11-16 bsp_uart (1)
- 前瞻：bsp_uart已放入BSP/UART（bsp_uart_init未开始写 第 3 级开中断接收时再填）
- 细节:Send函数封装HAL_UART_Transmit,其不同放回值有不同颜色反馈（-1/-2 两个分支还没验证）
- 方式和操作：绿灯闪两下后转蓝灯常亮，按键时蜂鸣器响，VOFA发123收123 发321收321
- 结果和证据：上板，VOFA 115200，按键后收到 D，灯变成 (230,145,56)，过了300ms回到蓝色灯（代码300ms）
- 故障纠正：Send函数返回值改变的颜色代码中的color_tick 用了 uint8_t 保存 HAL_GetTick() 的返回值 被截断 计时错误
- 教训：用来保存 HAL_GetTick() 返回值的变量必须和它的返回类型 uint32_t（Tab键补全一定要二次检查）
- 欠账：-1/-2 分支没有在板上验证 回声仍然直接调 HAL（留到第 3 级）
- send超时固定100ms length>1152必超时（推导：10 bit/字节，86.8 µs/字节）待决 
## 2026-11-17 bsp_uart (2)
- 方式：上板、VOFA 115200，printf 打印 boot ok、count=%d、pi=%f
- 结果：boot ok、count=123 正常；加选项前 pi= 为空，加选项后 pi=3.140000
- 证据：nano.specs 所在行；FLASH 38544 → 46388 B
- 教训：__io_putchar少打了字母 这个错没有任何报错 是 syscalls.c里的弱声明挡住了 
- -u _printf_float 一开始放进了 target_link_directories 放错命令不报错 只是不生效
- 前一次 Flash 增量不能当证据 因为同时改了代码
- 欠账：send 的超时固定100 ms length > 1152超时 printf逐字符调send函数 不能在中断里用 -1/-2 分支没验证 上一次的两个乱码字节来源不明
## 2026-11-19 bsp_uart (3) 环缓冲
- 方式：上板VOFA115200+宿主机 printf 验证
- 配置：RX环缓冲SIZE=128/MASK=12 字节满后丢弃新的保留旧的
- 只有中断改动head 循环改动tail
- 中断回调结束再调用一次Receive_IT 不调用只能收一个字节
- 结果：hello/123 回声正常；400个字节连发全部返回
- 证据：VOFA 截图；发10个字节一个没丢 发51个字节丢1个 大概在35位  90个字节丢2个 大概在36位左右
- 因为send函数和getc函数每跑一圈就加一点误差 达到阈值就会丢
- 环缓冲后 400 全回
- 欠账：接收TX依然阻塞（IT/DMA 待遥测上量）；在ch6用DBUS时再组装成帧格式；receive() 阻塞版留调试口；
  -1/-2 分支未上板；printf不可用于中断；send超时100ms对应1152字节上限
## 2026-11-21 bsp_imu 
- 方式：上板VOFA115200，手动移动喵板观察六轴的值
- 操作：从机id识别和认证 datasheet（0x1E/0x0F）
- 证据：ACC_CONF=0xA8（bit7必须置1、ODR是低4位、BWP是高4位；datasheet第八页的0xA是高半字节）
- 结果：acc与gyro都能够正常启用 并且可以读出六轴的原始值
- 问题：acc需要使能两次 第一次容易被忽略 不然acc3轴值无法读出
- 欠账：温度未读、DRDY/EXTI、DMA、gyro 零偏标定（Ch6再做）
## 2026-11-22 bsp_can (限Internal loopback测试中)
- 方式：上板Internal LoopBack 模式 依旧VOFA115200
- 配置：FDCAN1 PD0/PD1，Classic帧，1Mbps（HSE24M/ presc1/TS1=17/TS2=6 采样点75%左右）
  RxFifo0=3/TxFifoQueue=3 元素，StdFilters=0 + 全局接受，NVIC IT0
- 结果：回环通过——每秒 TX id=0x123 [cnt,0xAA]，中断收到并打印d0递增
- 证据：VOFA连续RX行；psr LEC=0、ecr=0
- 教训1：TxFifoQueueElmtsNbr或者RxFifo0ElmtsNbr=0导致无邮箱或者无接收槽，接收帧的时候就会失败
- 教训2：ConfigGlobalFilter必须在HAL_FDCAN_Start 之前调用
- 教训3：本版HAL的DataLength=字节数（不是len<<16）之前写错导致 DLCtoBytes[] 越界
  消息RAM越界写导致HardFault（GDB定位在FDCAN_CopyMessageToRAM）
- 欠账：上CAN总线（使用Normal mode+AutoRetransmission Enable+第二节点+两个120Ω电阻抗干扰）；
  扩展帧、硬件过滤器、Bus-Off恢复策略
## 2026-11-24 Ch5 FreeRTOS
- 方式：上板 VOFA115200 uart1 SPI2 SPI6 CAN四种通信方式可以同时进行（伪同时）
- 配置：接口使用CMSIS-RTOSV2 tick 1000Hz，heap 32KB；4个任务（T1 20ms、task2 200ms、
  LED 500ms、IMU 100ms）；T1/LED/IMU 优先级 Above Normal/Normal/AboveNormal
- 结果：周期准确（200/100ms零漂移） 四路输出无插入 发送丢失LOST=0 IMU a[z]≈1000mg
- 证据：VOFA截图+灯闪视频
- 教训1：osDelay 相对延时误差累计（201/202ms）所以控制周期用osDelayUntil
- 教训2：高优先级任务会抢占低优先级任务的HAL_UART_Transmit导致HAL_BUSY整行丢失（实测LOST递增到50+）
- 教训3：osMutexNew在osKernelInitialize之前调导致BASEPRI常置导致SysTick 被屏蔽
- 教训4：printf逐字符send，mutex只保护到单字节 导致行内插入；改 bsp_uart_printf 整行发送
- 教训5 ：状态码0=成功，if(funcation()) 方向反了导致LOST/SENT统计颠倒
- 教训6 ：osDelayUntil必须在每圈末尾 不然漏掉会导致高优先级任务空转饿死低优先级
- 教训7：WS2812实测：把"灭"改成青色后仍出现绿，证明绿与代码无关 需在上电后发一帧有效数据覆盖.同理任何“灯不受控”现象先怀疑器件上电状态再怀疑代码
- 欠账：真总线CAN、UART发送异步化、姿态解算