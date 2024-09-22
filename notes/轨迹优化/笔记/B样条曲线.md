# B样条曲线

B样条曲线与贝塞尔曲线类似，也是由基函数和控制点组成，但它却更复杂，因为每个基函数影响的节点区间不同，也就是说某个基函数在特定区间才非0。并且基函数是通过递归公式定义的，称为Cox-de Boor递归公式。、

## 节点，控制点与基函数

$u_0 \leq u_2 \leq u_3 \leq \ldots \leq u_m$称为节点(knots)，其中某些节点可能相等。

设B样条曲线的次数为p，则Cox-deBoor递归公式为：

![img](http://www.cs.mtu.edu/%7Eshene/COURSES/cs3621/NOTES/spline/B-spline/bs-basis.jpg)

整个B样条曲线的定义为：
$$
C(u)=\sum_{i=0}^{n}{N_{i,p}(u)}P_i
$$
其中$Pi$为控制点。

​	对于次数为0的情况，基函数是一系列阶梯函数。对于P大于0的情况，使用三角计算格式理解。

![img](http://www.cs.mtu.edu/%7Eshene/COURSES/cs3621/NOTES/spline/B-spline/bs-scheme.jpg)

​	为了计算$N_{i,1}(u)$,需要计算$N_{i,0}和N_{i+1,0}(u)$。对于$N_{1,1}$来说，$N_{1,0}和N_{2,0}$分别在[u1,u2)和[u2,u3)非0，因此$N_{1,1}$在[u1,u3)非0。以此类推可以根据该三角图推得每个基函数的非0区间以及每个区间的影响范围。

​	事实上对于$N_{i,p}$来说，他的非零区间为$[u_i,u_{i+p+1})$，也可以说$N_{i,p}$在p+1个节点区间非0。

## TODO

- 为什么B样条曲线不需要跟贝塞尔曲线一样取2*jerk - 1作为最高次数？他的控制点和次数是怎么选取的？
- 为什么他能通过首尾点？

## 重要性质

1. 设节点数为m+1，次数为p，控制点的个数（基函数的数目）为n+1，那么==m = n + p + 1==

2. 区间$[u_p,u_{m-p})$是所有基函数都不为0的区间，也称为有效区间，B样条曲线通常定义在这个区间上
3. $N_{i,p}$的非零区间为$[u_i,u_{i+p+1})$

## 导数

​	基函数的导数为：

![img](http://www.cs.mtu.edu/%7Eshene/COURSES/cs3621/NOTES/spline/B-spline/bspline-basis-derv.jpg)

​	该公式使得在计算p阶导数的时候可以不用计算p-1阶基函数的导数。将这些基函数代入到B样条曲线的定义式中即可得到B样条曲线的导数：

![image-20240922224022832](/home/yzh/.config/Typora/typora-user-images/image-20240922224022832.png)

​	$a_i=\frac{p}{u_{i+p}-u_i}$

​	由于函数定义在$[u_p,u_{m-p})$上，因此好像默认把$a_0和a_{n+1}$当成0来计算了，并且有一种常用的clamped B样条函数将前p个结点和后p个结点都设为重复结点以实现曲线经过首尾两点。

​	设$Q_i=\frac{p}{u_{i+p}-u_i}(P_i-P_{i-1})$，则
$$
C'(u)=\sum_{i=1}^{n}{N_{i,p-1}(u)}Q_i
$$
​	若设$Q_i=\frac{p}{u_{i+p+1}-u_{i+1}}(P_{i+1}-P_i)$，则
$$
C'(u)=\sum_{i=0}^{n-1}{N_{i+1,p-1}(u)}Q_i
$$
