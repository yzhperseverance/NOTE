# Actor-Critic算法

结合了DQN和策略梯度的想法，使用DQN学习价值函数，然后指导策略网络学习，最终的输出还是策略。

在策略梯度算法中，可以把梯度写成一个一般的形式：
![image-20241119204602666](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241119204602666.png)

其中$\psi_t$有多种形式：

![image-20241119204915828](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241119204915828.png)

对于形式4，可以考虑DQN学习价值函数的方法，使用策略网络+价值网络共同指导策略的学习。

![image-20241119205304725](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241119205304725.png)