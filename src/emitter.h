#ifndef _TIMETABLE_EMITTER_H
#define _TIMETABLE_EMITTER_H

#include <fstream>
#include <stack>
#include <string>

#include "schedule.h"

class Emitter {
public:
  Emitter(Schedule *schedule);

  void OpenTag(const std::string &name);
  void AddAttribute(const std::string &key, const std::string &value);
  void AddContent(const std::string &content);
  void CloseTag();

  void AddCell(const std::string &content = "", int merge_down = 0,
               int index = 0, bool is_number = false);
  void OutputTimetable();
  void OutputSectionTable();
  void OutputTeacherTable();
  
  void OutputSchedule(const std::string &file);

private:
  Schedule* schedule_;

  std::ofstream out_file_;
  bool start_tag_closed_;

  std::stack<std::string> open_tags_;
};

#endif