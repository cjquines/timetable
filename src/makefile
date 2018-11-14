CXXFLAGS = -g -O2 -pipe -std=c++14 -Wall
CNSTR = constraints/distinctperday.o \
	constraints/nonsimultaneous.o \
	constraints/reqfirstsubject.o \
	constraints/subjectgaps.o \
	constraints/subjecttime.o \
	constraints/teachertime.o
OBJS = main.o group.o schedule.o section.o subject.o teacher.o constraints/constraint.o

main: $(OBJS) $(CNSTR)
	g++ $(CXXFLAGS) -o main $(OBJS) $(CNSTR)

$(CNSTR) schedule.o: constraints/constraint.o
main.o: schedule.o group.o
group.o: section.o subject.o

clean:
	rm -f $(OBJS) $(CNSTR)
distclean: clean
	rm -f main main.exe