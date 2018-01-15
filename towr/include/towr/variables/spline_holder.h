/*
 * spline_holder.h
 *
 *  Created on: Jan 9, 2018
 *      Author: winklera
 */

#ifndef TOWR_TOWR_INCLUDE_TOWR_VARIABLES_SPLINE_HOLDER_H_
#define TOWR_TOWR_INCLUDE_TOWR_VARIABLES_SPLINE_HOLDER_H_

#include "contact_schedule.h"
#include "node_variables.h"
#include "spline.h"


namespace towr {

/**
 * This class is responsible for holding Pointers to fully constructed
 * splines, that are linked to the optimization variables.
 *
 * This is independent from whether they are added as optimization variables
 */
class SplineHolder {
public:
  using EndeffectorID = uint;


  // smell remove this
  SplineHolder () = default;
  virtual ~SplineHolder () = default;


  SplineHolder (NodeVariables::Ptr base_lin,
                NodeVariables::Ptr base_ang,
                const std::vector<double>& base_poly_durations,
                std::vector<NodeVariables::Ptr> ee_motion,
                std::vector<NodeVariables::Ptr> ee_force,
                std::vector<ContactSchedule::Ptr> contact_schedule,
                bool ee_durations_change);


  Spline::Ptr GetBaseLinear() const { return base_linear_; };
  Spline::Ptr GetBaseAngular() const { return base_angular_; };

  std::vector<Spline::Ptr> GetEEMotion() const { return ee_motion_; };
  std::vector<Spline::Ptr> GetEEForce()  const { return ee_force_; };
  Spline::Ptr GetEEMotion(EndeffectorID ee) const { return ee_motion_.at(ee); };
  Spline::Ptr GetEEForce(EndeffectorID ee)  const { return ee_force_.at(ee); };


private:
  Spline::Ptr base_linear_;
  Spline::Ptr base_angular_;

  std::vector<Spline::Ptr> ee_motion_;
  std::vector<Spline::Ptr> ee_force_;
};

} /* namespace towr */

#endif /* TOWR_TOWR_INCLUDE_TOWR_VARIABLES_SPLINE_HOLDER_H_ */