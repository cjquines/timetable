#ifndef _TIMETABLE_CONSTRAINTS_TEACHERBREAK_TPP
#define _TIMETABLE_CONSTRAINTS_TEACHERBREAK_TPP

template <typename T>
int TeacherBreak::ScoreTemplate(int teacher, int lbound, T countfunc) {
  if (teacher == 0) return 0;
  int num_free = 0, result = length_;
  for (int i = lbound + lbound_; i < lbound + rbound_; i++) {
    int count = countfunc(teacher, i);
    if (count > 0) {
      if (result > length_ - num_free) result = length_ - num_free;
      num_free = 0;
    } else num_free++;
  }
  if (result > length_ - num_free) result = length_ - num_free;
  if (result < 0) result = 0;
  return result;
}

#endif