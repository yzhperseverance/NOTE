/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file dp_road_graph.h
 **/

#include "modules/planning/tasks/dp_poly_path/dp_road_graph.h"

#include <algorithm>
#include <limits>
#include <string>
#include <unordered_map>
#include <utility>

#include "modules/common/proto/error_code.pb.h"
#include "modules/common/proto/pnc_point.pb.h"

#include "modules/common/configs/vehicle_config_helper.h"
#include "modules/common/log.h"
#include "modules/common/util/util.h"
#include "modules/planning/common/path/frenet_frame_path.h"
#include "modules/planning/common/planning_gflags.h"
#include "modules/planning/math/curve1d/quintic_polynomial_curve1d.h"
#include "modules/planning/math/double.h"
#include "modules/planning/tasks/dp_poly_path/trajectory_cost.h"

namespace apollo {
namespace planning {

using apollo::common::ErrorCode;
using apollo::common::Status;

DPRoadGraph::DPRoadGraph(const DpPolyPathConfig &config,
                         const ReferenceLine &reference_line,
                         const SpeedData &speed_data)
    : config_(config),
      reference_line_(reference_line),
      speed_data_(speed_data) {}

bool DPRoadGraph::FindPathTunnel(
    const common::TrajectoryPoint &init_point,
    const std::vector<const PathObstacle *> &obstacles,
    PathData *const path_data) {
  CHECK_NOTNULL(path_data);
  init_point_ = init_point;
  if (!reference_line_.XYToSL(
          {init_point_.path_point().x(), init_point_.path_point().y()},
          &init_sl_point_)) {
    AERROR << "Fail to create init_sl_point from : "
           << init_point.DebugString();
    return false;
  }
  std::vector<DPRoadGraphNode> min_cost_path;
  if (!GenerateMinCostPath(obstacles, &min_cost_path)) {
    AERROR << "Fail to generate graph!";
    return false;
  }
  // 采样最优路径上的点，并记录在frenet坐标系下
  std::vector<common::FrenetFramePoint> frenet_path;
  double accumulated_s = init_sl_point_.s();
  const double path_resolution = config_.path_resolution();

  for (std::size_t i = 1; i < min_cost_path.size(); ++i) {
    const auto &prev_node = min_cost_path[i - 1];
    const auto &cur_node = min_cost_path[i];

    const double path_length = cur_node.sl_point.s() - prev_node.sl_point.s();
    double current_s = 0.0;
    const auto &curve = cur_node.min_cost_curve;
    while (Double::Compare(current_s, path_length) < 0.0) {
      const double l = curve.Evaluate(0, current_s);
      const double dl = curve.Evaluate(1, current_s);
      const double ddl = curve.Evaluate(2, current_s);
      common::FrenetFramePoint frenet_frame_point;
      frenet_frame_point.set_s(accumulated_s + current_s);
      frenet_frame_point.set_l(l);
      frenet_frame_point.set_dl(dl);
      frenet_frame_point.set_ddl(ddl);
      frenet_path.push_back(std::move(frenet_frame_point));
      current_s += path_resolution;
    }
    accumulated_s += path_length;
  }
  FrenetFramePath tunnel(frenet_path);
  path_data->SetReferenceLine(&reference_line_);
  path_data->SetFrenetPath(tunnel);
  return true;
}

bool DPRoadGraph::GenerateMinCostPath(
    const std::vector<const PathObstacle *> &obstacles,
    std::vector<DPRoadGraphNode> *min_cost_path) {
  CHECK(min_cost_path != nullptr);

  std::vector<std::vector<common::SLPoint>> path_waypoints;
  if (!SamplePathWaypoints(init_point_, &path_waypoints)) {
    AERROR << "Fail to sample path waypoints!";
    return false;
  }
  // sample的时候没加初始点
  path_waypoints.insert(path_waypoints.begin(),
                        std::vector<common::SLPoint>{init_sl_point_});

  const auto &vehicle_config =
      common::VehicleConfigHelper::instance()->GetConfig();

  TrajectoryCost trajectory_cost(config_, reference_line_, obstacles,
                                 vehicle_config.vehicle_param(), speed_data_);

  std::vector<std::vector<DPRoadGraphNode>> graph_nodes(path_waypoints.size());
  graph_nodes[0].emplace_back(init_sl_point_, nullptr, 0.0);

  for (std::size_t level = 1; level < path_waypoints.size(); ++level) {
    const auto &prev_dp_nodes = graph_nodes[level - 1];
    const auto &level_points = path_waypoints[level];
    for (const auto &cur_point : level_points) {
      graph_nodes[level].emplace_back(cur_point, nullptr); // 生成一个对象并加入到graph_nodes[level]中
      auto &cur_node = graph_nodes[level].back();
      for (const auto &prev_dp_node : prev_dp_nodes) {
        const auto &prev_sl_point = prev_dp_node.sl_point;
        // 计算从前一个点到当前点的五次多项式曲线,一阶和二阶导为0表示路径的起点和终点与参考路径的方向相同
        QuinticPolynomialCurve1d curve(prev_sl_point.l(), 0.0, 0.0,
                                       cur_point.l(), 0.0, 0.0,
                                       cur_point.s() - prev_sl_point.s());
        const double cost =
            trajectory_cost.Calculate(curve, prev_sl_point.s(), cur_point.s()) +
            prev_dp_node.min_cost;
        cur_node.UpdateCost(&prev_dp_node, curve, cost);啊
      }
    }
  }

  // find best path
  DPRoadGraphNode fake_head;
  for (const auto &cur_dp_node : graph_nodes.back()) {
    // 上面不都update完了吗，干嘛还要update==>这里是为了初始化fake_head这个对象里的min_cost和min_cost_prev_node，用最后一层的数据
    fake_head.UpdateCost(&cur_dp_node, cur_dp_node.min_cost_curve,
                         cur_dp_node.min_cost);
  }

  // 从后往前遍历
  const auto *min_cost_node = &fake_head;
  while (min_cost_node->min_cost_prev_node) {
    min_cost_node = min_cost_node->min_cost_prev_node;
    min_cost_path->push_back(*min_cost_node);
  }
  std::reverse(min_cost_path->begin(), min_cost_path->end());
  return true;
}

bool DPRoadGraph::SamplePathWaypoints(
    const common::TrajectoryPoint &init_point,
    std::vector<std::vector<common::SLPoint>> *const points) {
  CHECK(points != nullptr);

  common::math::Vec2d init_cartesian_point(init_point.path_point().x(),
                                           init_point.path_point().y());
  // 不是已经有一个init_sl_point_，干嘛还要再生成一个
  common::SLPoint init_sl_point;
  if (!reference_line_.XYToSL(init_cartesian_point, &init_sl_point)) {
    AERROR << "Failed to get sl point from point "
           << init_cartesian_point.DebugString();
    return false;
  }

  const double reference_line_length =
      reference_line_.map_path().accumulated_s().back();

  // 用初始点的速度当作步长，初始点是上一帧轨迹的点吗？
  double level_distance =
      std::fmax(config_.step_length_min(),
                std::fmin(init_point.v(), config_.step_length_max()));

  double accumulated_s = init_sl_point.s();
  for (std::size_t i = 0;
       i < config_.sample_level() && accumulated_s < reference_line_length;
       ++i) {
    std::vector<common::SLPoint> level_points; // 每个采样步长s对应一个level
    accumulated_s += level_distance;
    double s = std::fmin(accumulated_s, reference_line_length);

    int32_t num =
        static_cast<int32_t>(config_.sample_points_num_each_level() / 2);

    // 在横向上采样
    for (int32_t j = -num; j < num + 1; ++j) {
      double l = config_.lateral_sample_offset() * j; // 横向偏移量
      auto sl = common::util::MakeSLPoint(s, l);
      if (reference_line_.IsOnRoad(sl)) {
        level_points.push_back(sl);
      }
    }
    if (!level_points.empty()) {
      points->push_back(level_points);
    }
  }
  return true;
}

}  // namespace planning
}  // namespace apollo
