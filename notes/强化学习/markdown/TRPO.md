# TRPO

基于策略的优化目标为：
$$
J(\theta)=E_{s_0}[V^{\pi_\theta}(s_0)]=E_{\pi_\theta}[\sum_{t=0}^\inf \gamma^tr(s_t,a_t)]
$$
第一个E可以理解为在不同初始状态分布下使用策略$\pi_\theta$的价值期望，即初始状态不同得到的期望价值。

第二个E是将V展开成$E_{\pi_\theta}[G_t|s_0]$的形式，最后的结果省略了s0，但其实还是隐含着初始状态分布。



TRPO的想法是设立一个信任区域，使得在这个信任区域上进行策略更新时能够保证策略是越来越好的。

## 公式推导

![image-20241120201631266](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241120201631266.png)

这里其实下标无所谓的，沿用s0也可以，因为期望内的部分跟$\pi_{\theta’}$五关。重点是将里面用两项差的形式展开了。

![image-20241120201756658](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241120201756658.png)

这里就是构造出了时序差分的形式，将其定义为优势函数A，如果优势函数A>0，表示当前动作比之前更优，反之更劣，因为优势函数前两项是由Gt变换来的，V（st）则是表示之前在该状态产生的价值。

![image-20241120202410230](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241120202410230.png)

状态访问分布表示在策略$\pi$下到达状态s的概率。

![image-20241120202455886](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241120202455886.png)

使用当前策略的状态访问分布替代新策略的状态访问分布，即在每个状态访问的概率。

![image-20241120202806087](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241120202806087.png)

重要性采样可以表示两个策略的差异，这里通过引入这个将动作a也变为从当前策略$\pi_\theta$下选取。

![image-20241120202915656](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241120202915656.png)

KL散度就是评估两个分布之间的变化量的，这里要求两个分布变化量在一定区间内，即“信任区域”。因为如果变化太大的话新旧策略之间的状态访问分布就不能近似看成一样的了。

## 求解

最终的更新方程其实是共轭梯度那步关于x的式子，而要求x则要用到g和H，也就需要求优势函数和KL散度。

![image-20241120203045197](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241120203045197.png)

![image-20241120203105790](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241120203105790.png)

![image-20241120203116478](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241120203116478.png)

## 优势函数求解

![image-20241120204039646](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241120204039646.png)

```python
def compute_advantage(gamma, lmbda, td_delta):
    td_delta = td_delta.detach().numpy()
    advantage_list = []
    advantage = 0.0
    for delta in td_delta[::-1]:
        advantage = gamma * lmbda * advantage + delta
        advantage_list.append(advantage)
    advantage_list.reverse()
    return torch.tensor(advantage_list, dtype=torch.float)
```

这段代码是在根据GAE公式逆向求解，即
$$
A_t=\delta_t + \gamma \lambda A_{t+1}
$$


