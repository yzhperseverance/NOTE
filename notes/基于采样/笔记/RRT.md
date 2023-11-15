# RRT

TODO:看看原论文，对终点是怎么处理的，如果没采样到终点呢，或者在构建初期就采样到了终点？

第一个问题：不需要精确采样到终点，终点的邻域即可

第二个问题：无所谓，因为采样点只是提供一个方向，并不是直接连接两个点，而是基于一个给定的长度朝采样点的方向作连线

## 基本思想

​	通过将采样点构建成一个树，这样在构建到终点时由于树的特性自然而然就生成了一条路径。

![1698240547719](C:\Users\DELL-PC\AppData\Roaming\Typora\typora-user-images\1698240547719.png)

## 具体流程

1. 生成采样点x<sub>rand</sub>
2. 找到树中与x<sub>rand</sub>距离最近的点x<sub>near</sub>
3. 在x<sub>rand</sub>和x<sub>near</sub>的连线中的一段距离生成一个点x<sub>new</sub>，将x<sub>new</sub>加入树中，父结点为x<sub>near</sub>
4. 如果x<sub>new</sub>在障碍物中，则跳过
5. 如果终点或终点的邻域内的点被采样到并加入树中，此时搜索结束

## 局限场景

![1698241441660](C:\Users\DELL-PC\AppData\Roaming\Typora\typora-user-images\1698241441660.png)

​     在一些很窄的地方，称为narrow passage，由于区域面积很小，采样点很难到里面，此时RRT的扩展速度会很慢，称为

## 一些优化手段

1. 使用KD-Tree搜索离x<sub>rand</sub>最近的点
2. Bidirectional RRT：从起点和终点两头同时生成两棵树，当两棵树连接时，搜索结束
3. 主流：通过设计采样函数去解决narrow  Passage问题（再查查别的方法的论文）

