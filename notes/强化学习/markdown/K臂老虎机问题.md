# K臂老虎机问题

![image-20241118193953807](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241118193953807.png)

![image-20241118194057477](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241118194057477.png)

## 估计期望奖励

### 批量式

$$
Q_k=\frac{1}{k}\sum_{i=1}^{k}r_i
$$

### 增量式

![image-20241118194324170](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241118194324170.png)

## 探索与利用的平衡

由于事先不知道每个杆所对应的奖励，因此需要不断拉杆探索计算期望，但当次数是有限的时候，就需要去平衡探索和利用的次数，因为最终是想要奖励尽可能高，如果一直探索而不去选择一个最优的，就会使奖励偏低。

### $\epsilon-Greedy$算法

![image-20241118194625035](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241118194625035.png)

### 上置信界算法（UCB）

![image-20241118195655211](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241118195655211.png)

![image-20241118195837213](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241118195837213.png)