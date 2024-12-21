# Q-Learning

​	与Sarsa算法类似，修改了时序差分的更新方式。Sarsa算法的Q(st+1,at+1)依赖策略，当策略换了则下个状态也不一样，而Q-learning由于取的是动作值函数的最大值，所以他不依赖策略，这种称为离线策略算法。并且在进行完一个动作之后即可进行更新，并不需要等待整个序列更新完。

![image-20241118204244051](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241118204244051.png)