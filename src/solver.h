#ifndef _TIMETABLE_SOLVER_H
#define _TIMETABLE_SOLVER_H

#include <algorithm>
#include <map>
#include <random>
#include <utility>
#include <vector>

#include "section.h"
#include "schedule.h"

class Solver {
public:
  Solver(Schedule* schedule, int seed);

  bool InitialSchedule();

  template <typename T, typename U, typename V>
  int SearchTemplate(T translate, U swap, V adjswap);

  template <typename T, typename U, typename V>
  int HardSearchTemplate(T translate, U swap, V adjswap);

  template <typename T, typename U, typename V>
  int SoftSearchTemplate(T translate, U swap, V adjswap);

  int HardSolver(int time_limit);
  int HardLocalSearch();
  int HardTabuSearch();

  int SoftSolver(int time_limit, int num_samples, double kappa, int tau,
                 double alpha);
  int SoftLocalSearch(bool accept_side, int threshold);
  int SoftSimulatedAnnealing(int time_limit, int num_samples, double kappa,
                             int tau, double alpha);
  double SimulatedAnnealingSample(int num_samples);
  int SimulatedAnnealingSearch(double temperature);

  int Solve(int time_limit, int attempts, int max_best, int num_samples,
            double kappa, int tau, double alpha);
  std::pair<int, Schedule*> GetBestSchedule(int schedule);

private:
  Schedule* schedule_;
  std::mt19937 rand_generator_;

  std::map<int, Schedule*> best_schedules_;
  std::vector< std::vector<bool> > subject_tabus_;
};

#include "solver.tpp"

#endif