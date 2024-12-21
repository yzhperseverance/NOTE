# Sarsa算法

## 单步Sarsa

​	时序差分算法基于价值迭代，Sarsa则基于策略迭代。注意，这里并不像动态规划的策略迭代那样要更新所有状态的价值函数，因为它是无模型的，并且动作价值的估计只依赖执行这个动作的单步reward和之前的动作价值。

![image-20241118202721504](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241118202721504.png)

## 多步Sarsa

![image-20241118204035037](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241118204035037.png)