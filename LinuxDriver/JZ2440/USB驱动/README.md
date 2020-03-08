# USB设备驱动
1. USB协议在linux内核中的USB总线驱动已经实现
2. 该代码只是参考内核其他的USB设备驱动（路径：`drivers/hid/usbhid/usbmouse.c`），写出一个具有输入子系统功能的USB设备（鼠标）的USB设备驱动
3. 该设备驱动，默认左键对应事件`KEY_L`,右键对应事件`KEY_S`，滚轮`KEY_ENTER`，检测这三个事件（从`buffer[0]`中获取），可以通过app直接读取/dev/input/event0，即可以把该三个事件的值读出。