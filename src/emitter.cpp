#include <fstream>
#include <stack>
#include <string>

#include "emitter.h"
#include "section.h"
#include "schedule.h"
#include "subject.h"
#include "teacher.h"

const std::string kColumnWidth = "105";
const std::string kRowHeight = "31.5";

Emitter::Emitter(Schedule *schedule)
    : schedule_(schedule), start_tag_closed_(true) {}

void Emitter::OpenTag(const std::string &name) {
  if (!start_tag_closed_) {
    out_file_ << ">";
    start_tag_closed_ = true;
  }
  out_file_ << "<" << name;
  start_tag_closed_ = false;
  open_tags_.push(name);
}

void Emitter::AddAttribute(const std::string &key, const std::string &value) {
  out_file_ << " " << key << "=\"" << value << "\"";
}

void Emitter::AddContent(const std::string &content) {
  if (!start_tag_closed_) {
    out_file_ << ">";
    start_tag_closed_ = true;
  }
  out_file_ << content;
}

void Emitter::CloseTag() {
  if (!start_tag_closed_) {
    out_file_ << ">";
    start_tag_closed_ = true;
  }
  out_file_ << "</" << open_tags_.top() << ">";
  open_tags_.pop();
}

void Emitter::AddCell(const std::string &content, int merge_down, int index,
                      bool is_number) {
  OpenTag("Cell");
  if (index > 0) AddAttribute("ss:Index", std::to_string(index));
  if (merge_down > 0) AddAttribute("ss:MergeDown", std::to_string(merge_down));
  OpenTag("Data");
  if (is_number) AddAttribute("ss:Type", "Number");
  else AddAttribute("ss:Type", "String");
  AddContent(content);
  CloseTag();
  CloseTag();
}

void Emitter::OutputTimetable() {
  int num_sections = schedule_->GetSections().size();
  for (int i = 0; i < schedule_->GetNumDays(); i++) {
    OpenTag("Worksheet");
    AddAttribute("ss:Name", "Day " + std::to_string(i + 1));
    OpenTag("Table");
    AddAttribute("ss:DefaultColumnWidth", kColumnWidth);
    AddAttribute("ss:DefaultRowHeight", kRowHeight);

    OpenTag("Row");
    AddCell();
    for (int j = 0; j < num_sections; j++)
      AddCell(schedule_->GetSection(j)->GetName());
    CloseTag();

    for (int j = i*schedule_->GetNumSlotsPerDay();
         j < (i+1)*schedule_->GetNumSlotsPerDay(); j++) {
      OpenTag("Row");
      AddCell(std::to_string(j), 0, 1, true);
      for (int k = 0; k < num_sections; k++) {
        int subject = schedule_->GetSubjectOf(k, j);
        if (subject < 0) continue;
        int teacher = schedule_->GetTeacherOf(k, j);
        AddCell(schedule_->GetSubject(subject)->GetName()
              + "&#10;" + schedule_->GetTeacher(teacher)->GetName(),
                schedule_->GetLengthOf(k, j) - 1, k + 2);
      }
      CloseTag();
    }

    CloseTag();
    CloseTag();
  }
}

void Emitter::OutputSectionTable() {
  for (const auto &it : schedule_->GetSections()) {
    OpenTag("Worksheet");
    AddAttribute("ss:Name", it->GetName());
    OpenTag("Table");
    AddAttribute("ss:DefaultColumnWidth", kColumnWidth);
    AddAttribute("ss:DefaultRowHeight", kRowHeight);

    OpenTag("Row");
    AddCell();
    for (int i = 0; i < schedule_->GetNumDays(); i++)
      AddCell("Day " + std::to_string(i + 1));
    CloseTag();

    for (int i = 0; i < schedule_->GetNumSlotsPerDay(); i++) {
      OpenTag("Row");
      AddCell(std::to_string(i), 0, 1, true);
      for (int j = 0; j < schedule_->GetNumDays(); j++) {
        int timeslot = j*schedule_->GetNumSlotsPerDay() + i;
        int subject = schedule_->GetSubjectOf(it->GetId(), timeslot);
        if (subject < 0) continue;
        int teacher = schedule_->GetTeacherOf(it->GetId(), timeslot);
        AddCell(schedule_->GetSubject(subject)->GetName()
              + "&#10;" + schedule_->GetTeacher(teacher)->GetName(),
                schedule_->GetLengthOf(it->GetId(), timeslot) - 1, j + 2);
      }
      CloseTag();
    }

    CloseTag();
    CloseTag();
  }
}

void Emitter::OutputTeacherTable() {
  for (const auto &it : schedule_->GetTeachers()) {
    OpenTag("Worksheet");
    AddAttribute("ss:Name", it->GetName());
    OpenTag("Table");
    AddAttribute("ss:DefaultColumnWidth", kColumnWidth);
    AddAttribute("ss:DefaultRowHeight", kRowHeight);

    OpenTag("Row");
    AddCell();
    for (int i = 0; i < schedule_->GetNumDays(); i++)
      AddCell("Day " + std::to_string(i + 1));
    CloseTag();

    for (int i = 0; i < schedule_->GetNumSlotsPerDay(); i++) {
      OpenTag("Row");
      AddCell(std::to_string(i), 0, 1, true);
      for (int j = 0; j < schedule_->GetNumDays(); j++) {
        int timeslot = j*schedule_->GetNumSlotsPerDay() + i;
        int section = schedule_->GetSectionOf(it->GetId(), timeslot);
        if (section < 0) continue;
        int subject = schedule_->GetSubjectOf(section, timeslot);
        if (subject < 0) continue;
        AddCell(schedule_->GetSubject(subject)->GetName()
              + "&#10;" + schedule_->GetSection(section)->GetName(),
                schedule_->GetLengthOf(section, timeslot) - 1, j + 2);
      }
      CloseTag();
    }

    CloseTag();
    CloseTag();
  }
}

void Emitter::OutputSchedule(const std::string &file) {
  out_file_.open(file);
  out_file_ << "<?xml version=\"1.0\"?>";
  out_file_ << "<?mso-application progid=\"Excel.Sheet\"?>";

  OpenTag("Workbook");
  AddAttribute("xmlns", "urn:schemas-microsoft-com:office:spreadsheet");
  AddAttribute("xmlns:o", "urn:schemas-microsoft-com:office:office");
  AddAttribute("xmlns:x", "urn:schemas-microsoft-com:office:excel");
  AddAttribute("xmlns:ss", "urn:schemas-microsoft-com:office:spreadsheet");
  AddAttribute("xmlns:html", "http://www.w3.org/TR/REC-html40");
  
  OpenTag("DocumentProperties");
  AddAttribute("Author", "cjquines/timetable");
  CloseTag();

  OpenTag("ExcelWorkbook");
  AddAttribute("xmlns", "urn:schemas-microsoft-com:office:excel");
  CloseTag();

  OpenTag("Styles");
  OpenTag("Style");
  AddAttribute("ss:ID", "Default");
  OpenTag("Alignment");
  AddAttribute("ss:Horizontal", "Center");
  AddAttribute("ss:Vertical", "Center");
  AddAttribute("ss:WrapText", "1");
  CloseTag();
  CloseTag();
  CloseTag();

  OutputTimetable();
  OutputSectionTable();
  OutputTeacherTable();

  CloseTag();
  out_file_.close();
}