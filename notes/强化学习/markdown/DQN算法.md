# DQN算法

​	正式进入使用神经网络的阶段。由于Q_learning的存表方式只适合离散且空间小的情况，因此DQN提出使用神经网络去拟合Q值。

​	**DQN只能应用在离散的动作空间中，因为他要取max**

​	DQN使用Q_learning的时序差分向构建损失函数：

![image-20241119193817608](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241119193817608.png)