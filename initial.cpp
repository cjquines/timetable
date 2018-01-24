#include <cassert>
#include <iostream>
#include <vector>

int num_days, slots_per_day, num_slots;
int num_groups; // repl num_whatevers with vectors
int num_sections; // vector<Section> sections_;
int num_subjects;
int num_teachers;
std::vector<int> subject_num_slots;
std::vector<int> subject_teachers;
std::vector< std::vector<int> > timetable;
std::vector< std::vector<int> > teacher_table;
std::vector<int> num_teacher_conflicts;

int CountHardConstraints() {
  int result = 0;
  for (int i = 0; i < num_teachers; i++)
    result += num_teacher_conflicts[i];
  return result;
}

void AssignSubject(int subject, int section, int slot) {
  assert(timetable[section][slot] == -1);
  int teacher = subject_teachers[subject];
  if (teacher_table[teacher][slot] != -1)
    num_teacher_conflicts[teacher] += 1;
  teacher_table[teacher][slot] += 1;
  timetable[section][slot] = subject;
}

void InitialAssignSection(int section) {
  std::vector<int> unassigned_subjects;
  for (int i = 0; i < num_subjects; i++) {
    for (int j = 0; j < subject_num_slots[i]; j++)
      unassigned_subjects.push_back(i);
  }

  while (!unassigned_subjects.empty()) {
    int to_assign = unassigned_subjects.back();
    int teacher = subject_teachers[to_assign];
    int min = 100000, min_slot = -1; // inf?
    for (int i = 0; i < num_slots; i++) {
      if (timetable[section][i] != -1) continue;
      int num_conflicts = teacher_table[teacher][i];
      if (num_conflicts < min) {
        min = num_conflicts;
        min_slot = i;
      }
    }
    AssignSubject(to_assign, section, min_slot);
    unassigned_subjects.pop_back();
  }
}

void InitialAssign() {
  for (int i = 0; i < num_sections; i++)
    InitialAssignSection(i);
}

void SwapTimeslots(int lhs, int rhs) {

}

void PrintAssignment() {
  std::cout << "SECTIONS\n";
  for (int i = 0; i < num_sections; i++) {
    std::cout << i << ' ';
    for (int j = 0; j < num_slots; j++) {
      std::cout << timetable[i][j] << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << "\nTEACHERS\n";
  for (int i = 0; i < num_teachers; i++) {
    std::cout << i << ' ';
    for (int j = 0; j < num_slots; j++) {
      std::cout << teacher_table[i][j] << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

int main() {
  std::cin >> num_days >> slots_per_day;
  num_slots = num_days * slots_per_day;

  std::cin >> num_teachers;
  std::cin >> num_groups; // for each group:
  std::cin >> num_sections;
  std::cin >> num_subjects;

  subject_num_slots.assign(num_subjects, 0);
  subject_teachers.assign(num_subjects, 0);
  for (int i = 0; i < num_subjects; i++)
    std::cin >> subject_num_slots[i] >> subject_teachers[i];

  timetable.assign(num_sections, std::vector<int>(num_slots, -1));

  teacher_table.assign(num_teachers, std::vector<int>(num_slots, -1));

  num_teacher_conflicts.assign(num_teachers, 0);

  InitialAssign();
  PrintAssignment();
  std::cout << CountHardConstraints() << std::endl;
}