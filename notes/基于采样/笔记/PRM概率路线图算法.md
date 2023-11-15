# Probabilistic Road Map(PRM概率路线图算法)

## 基本思想

1. learning phase：在空间中基于采样函数采样一堆点，然后将在障碍物的点去除，从起始点开始连接一定距离内的所有点，构成一个图

![1698238744524](C:\Users\DELL-PC\AppData\Roaming\Typora\typora-user-images\1698238744524.png)

2. query phase：用A*在图里搜索路径

![1698239401343](C:\Users\DELL-PC\AppData\Roaming\Typora\typora-user-images\1698239401343.png)

## 优缺点

- 优点：概率完备，且相比栅格的节点数，图的节点数量更少
- 缺点：对一个路径规划问题拆分成了两个问题，不够高效，理论上在learning phase结束之后就应该计算出最优路径了，不需要额外的搜索



## 一种优化方法

​	PRM对于每个点都要检测是否在障碍物里，这个过程非常耗时，因此有人对这步做了一个优化，称为Lazy collision-checking。他的核心思想是只针对可能的最优路径进行collision-checking。

​	首先对于采样的所有点都跳过collision-checking，直接进行learning phase，然后在query phase中如果路径中有一点在障碍物中，则删除这个点及其相连的边，然后重新进行query phase（**这里我以为是对这个点相连的路径上的两点重新找一个路径，其实是对整条路径都重新找，这是否有点过于麻烦？但是可能在删除该点之后相连两点重新找到的路径并不是最优的了，所以为了最优可能确实还得整条路径都重新搜索。**）

​	![1698240227844](C:\Users\DELL-PC\AppData\Roaming\Typora\typora-user-images\1698240227844.png)



