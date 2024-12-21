# EM Planner总结（apollo 1.5）

规划模块和预测模块是分开的，规划模块会直接接收当前帧预测的障碍物位置。

## 规划流程

整个规划模块由工厂模式控制，总共有6个任务：

```c++
void EMPlanner::RegisterTasks() {
  task_factory_.Register(TRAFFIC_DECIDER,
                         []() -> Task* { return new TrafficDecider(); });
  task_factory_.Register(DP_POLY_PATH_OPTIMIZER,
                         []() -> Task* { return new DpPolyPathOptimizer(); });
  task_factory_.Register(PATH_DECIDER,
                         []() -> Task* { return new PathDecider(); });
  task_factory_.Register(DP_ST_SPEED_OPTIMIZER,
                         []() -> Task* { return new DpStSpeedOptimizer(); });
  task_factory_.Register(QP_SPLINE_PATH_OPTIMIZER,
                         []() -> Task* { return new QpSplinePathOptimizer(); });
  task_factory_.Register(QP_SPLINE_ST_SPEED_OPTIMIZER, []() -> Task* {
    return new QpSplineStSpeedOptimizer();
  });
}
```

- **TRAFFIC_DECIDER**：在1.5版本主要是为了忽略后方一定距离和超出规划范围的障碍物
- **DP_POLY_PATH_OPTIMIZER**：SL的DP规划，每个level之间的点用五次多项式连接，计算代价
- **PATH_DECIDER**：根据障碍物的位置，判断对他的决策是ignore、停车或者nudge(轻推)
- **DP_ST_SPEED_OPTIMIZER**：ST的DP规划，建立ST图，表示不同时刻到达的位置，反映了速度的大小
- **QP_SPLINE_PATH_OPTIMIZER**：SL的QP规划
- **QP_SPLINE_ST_SPEED_OPTIMIZER**：ST的QP规划

## SL的DP规划

​	使用当前位置的速度作为采样的步长，不同level的横向步长是相同的。采样后的不同level之间的点使用五次多项式连接，并且一阶和二阶导设为0来计算五次多项式的系数，推测是想在每段路径的起点和终点都与参考线的方向相同。

​	这个五次多项式路径的代价由平滑代价和障碍物代价组成。平滑代价是每一点的l值和dl值。障碍物代价是每一点与障碍物的距离，这里需要注意的是代码里并不是用固定步长采样计算障碍物代价的，而是通过一个heuristic_speed和固定时间差采样的路径上的点，猜测是为了减少计算量？或者其他安全考虑？目前不清楚heuristic_speed是怎么得到的。

​	障碍物代价根据与障碍物的不同距离段代价函数不同。

## 障碍物决策

​	这里对每个障碍物进行决策，并对它的决策结果打上标签，用于后续ST的QP规划中作为boundary代价函数使用。

​	设置了一个缓冲区域=汽车宽度的一半 + 一个固定值，分为横向忽略缓冲区和横向停止缓冲区。如果障碍物在忽略缓冲区外，则忽略该障碍物。如果障碍物全部挡在停止缓冲区内，则施加停止决策。如果障碍物只有一侧挡住，则施加轻推（nudge）决策（在后续版本删除，改为提前变道）。

## ST的DP规划

​	**注意：此时经过SL的DP规划后，参考线已经变成了SL规划的轨迹，也就是当前路径的l坐标应该都为0。**

​	ST规划通过构建ST图表实现DP规划。通过SL规划的路径s总长度和总时间，分割出固定段，并构建出cost_table（dim_t x dim_s）。可以理解为不同时间到达的不同位置。

​	不同时间的节点的代价由三部分组成：障碍物代价、上一时刻的最小代价、上一时刻到当前时刻的代价。其中第三项包括速度代价、加速度代价和jerk代价，需要至少4个点计算，对于前三个点则进行特殊处理。

​	最后输出一串速度数据，包含st点。

## SL的QP规划

​	QP规划的点是基于SL的DP的结果计算的，但这其实是隐含信息，并没有明确写出来，因为SL的DP规划将参考线设置为了它输出的路径，而QP的控制点好像初始l值都是0，这其实就代表着整条路径的初值与DP的结果重合。

​	代价包括：始末点的代价、曲率代价、各种boundary的代价。

​	这里以点的代价举例：

```c++
bool Spline1dConstraint::AddPointConstraint(const double x, const double fx) {
  std::uint32_t index = FindIndex(x); // 找到x位于的区间
  std::vector<double> power_x;
  GeneratePowerX(x - x_knots_[index], spline_order_, &power_x);
  Eigen::MatrixXd equality_constraint =
      Eigen::MatrixXd::Zero(1, (x_knots_.size() - 1) * spline_order_);
  std::uint32_t index_offset = index * spline_order_; // 给指定段的控制点增加约束
  for (std::uint32_t i = 0; i < spline_order_; ++i) {
    equality_constraint(0, index_offset + i) = power_x[i];
  }
  Eigen::MatrixXd equality_boundary(1, 1);
  equality_boundary(0, 0) = fx;
  return AddEqualityConstraint(equality_constraint, equality_boundary);
}
```

​	power_x是x的不同幂次，从1到$x^{order}$，这里用的样条貌似跟B样条并不一样，而是跟分段多项式类似，每段通过控制点计算点的方式跟多项式相同。

​	这里的曲率代价好像是通过限制二阶导的上下界施加的，可能是一种近似。

## ST的QP规划

​	代价：初始点位置和速度、终点加速度=0、s的单调性、二阶导的连续性、boundary、速度限制、加速度限制。

​	==目前没有找到它是否利用了ST的DP输出作为初值，不太清楚ST的DP输出的作用== 