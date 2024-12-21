# DH表达法（Craig）

## 引言

​	对于一个机械臂，需要根据其各个关节的驱动角计算出手臂末态的状态点（位置、速度···），并且要根据手臂末态位置反算出各个关节的角，即：$w_p=f(\theta_1, \theta_2,...,\theta_n)$

![image-20241211180506181](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241211180506181.png)

## 手臂几何描述方式

### Joint

​	joint代表关节，每个joint规定只有一个自由度：转动或平移，每个joint对特定的axis进行旋转或平移

### Link

​	连接joint的杆件，是刚体。需要特别注意的是在头尾规定了两个类似指针虚拟头结点的link，主要是因为DH表达法中的坐标系和角的定义依赖前一个joint和后一个joint，因此要对头尾进行特殊处理。

![image-20241211181433101](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241211181433101.png)

## DH表达法（Craig version）

​	每个关节使用4个参数表示，两个长度和两个夹角。**注意这里$\alpha_{i-1}$和$a_{i-1}$是用来表示轴i的，并不是用来表示**

**i-1的。**

![image-20241211181611609](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241211181611609.png)

​	首先，对于关节来说，将z轴定义为它的转轴，x轴定义为当前转轴与下一个关节的转轴的公垂线的方向，如果两个转轴（z轴）在空间中相交，则将x轴定义为垂直于两个转轴所成平面的法向量方向（正负方向均可）。原点就定义在转轴与公垂线（x轴）的相交位置。

​	**DH表达法的核心思想在于：想要找到两个坐标系的变换关系，只需要找到两个轴之间的变换关系即可，即$z_{i-1}$和$z_i$的旋转和平移变换以及$x_{i-1}$和$x_i$的旋转和变换关系。**

​	对于第i个关节来说，其由两个在上一关节的参数和两个在当前关节的参数组成。$\alpha_{i-1}$和$a_{i-1}$定义了$z_{i-1}$到$z_i$的变换，$\theta_i$和$d_i$定义了$x_{i-1}$到$x_i$变换。（d其实就是原点在z方向上的距离差，a就是原点在x方向上的距离差）因此只需要通过欧拉角依次计算i-1到i的变换，即可求得i到i-1的变换矩阵，即$T_{i}^{i-1}$（这里看上去是反过来的，其实逻辑没有问题，从i-1按角计算变换到i求的就是i到i-1的变换矩阵，可以看左乘与右乘那篇笔记）。

![image-20241211184650441](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241211184650441.png)

## 变换矩阵的形式

![image-20241216122105293](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241216122105293.png)

## 开始和最后的关节

​	由于dh表示法中部分参数是依赖上一个关节的，并且x轴是依赖下一个关节，因此需要对开始（地杆）和最后一个节点进行特殊处理，其中地杆是固定不动的，而最后一个关节按我的理解一般是取机械手掌位置到转轴的连线作为x轴。

​	地杆的定义是当$\theta_1=0$时，需要使$Frame_1$与$Frame_0$重合，同理对于最后一个坐标系与$\theta_n$的定义是当$\theta_n=0$时，$Frame_n$与$Frame_{n-1}$重合。可以看出两个坐标系的定义主要是针对x轴的旋转，$\theta$定义的。对于地杆来说，关键在于找好原点，即那个不动的点。

![image-20241211185239047](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241211185239047.png)

​	如果第一个关节是平移关节，那么坐标系原点就不需要一定与第一个关节重合。比如下面这个例子就把地杆设在了最下面，而实际平移的关节则设置在了上面。**注意：除了地杆之外其他关节的坐标系都是动态的。**

![image-20241211185437940](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241211185437940.png)

​	对于z轴相交的情况，可以看下面的例子。

![image-20241211185721733](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241211185721733.png)

## Joint space, Cartesian space, Actuator space

Actuator space就是制动器的空间，比如马达的转速。一般来说制动器与关节的实际转速并不是1:1的，中间会经过减速机构，因此需要做相应的转换才能获得实际joint space下的转角和转速。

![image-20241211185832669](C:\Users\28609\AppData\Roaming\Typora\typora-user-images\image-20241211185832669.png)