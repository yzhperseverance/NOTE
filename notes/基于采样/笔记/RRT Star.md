# RRT Star

## 基本思想

在RRT的基础上，改进了连接x<sub>near</sub>和x<sub>new</sub>的逻辑，通过不断剪枝优化路径

## 流程

- 在得到x<sub>near</sub>和x<sub>new</sub>后，找出x<sub>new</sub>邻域的所有点，计算x<sub>new</sub>分别由这些点到达起点的路径距离，连接最小距离对应的点。

![image-20231027200303887](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20231027200303887.png)

- 连接后，比较邻域中的其他点X1、X2由他们原有父节点到达起点的距离短还是由x<sub>new</sub>到达起点的距离短，如果是后者，则剪掉他们与原父节点的连接，将x<sub>new</sub>作为他们的新的父节点

![image-20231027200608557](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20231027200608557.png)

## 其他在RRT*的基础上的改进算法

1. Kinodynamic-RRT*：生成的路径更平滑，两点间不单纯用直线连接
2. Anytime-RRT*：实时计算最优路径，不断更改、优化
3. Informed RRT*：在RRT Star已经找到一条路径后，并不仍沿用原来的全图采点方式，而是在一个包含路径的椭圆内进行采点，椭圆的焦距为路径的长度，由于路径在不断剪枝优化，因此该椭圆也在不断变化

![image-20231027201316151](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20231027201316151.png)