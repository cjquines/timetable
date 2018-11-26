CXXFLAGS = -g -O2 -pipe -std=c++14 -Wall
CNSTR = constraints/distinctperday.o \
	constraints/evendismissal.o \
	constraints/minsubjects.o \
	constraints/maxconsecutive.o \
	constraints/nonsimultaneous.o \
	constraints/reqfirstsubject.o \
	constraints/subjectgaps.o \
	constraints/subjecttime.o \
	constraints/teachertime.o
OBJS = emitter.o group.o parser.o schedule.o section.o \
	solver.o subject.o teacher.o constraints/constraint.o
STATICFLAGS = -static -static-libgcc -static-libstdc++

main: main.o $(OBJS) $(CNSTR)
	g++ $(CXXFLAGS) -o main main.o $(OBJS) $(CNSTR)

static: main.o $(OBJS) $(CNSTR)
	g++ $(STATICFLAGS) $(CXXFLAGS) -o timetable main.o $(OBJS) $(CNSTR)

tester: main-tester.o $(OBJS) $(CNSTR)
	g++ $(CXXFLAGS) -o tester main-tester.o $(OBJS) $(CNSTR)

main.o: emitter.o parser.o schedule.o solver.o
$(CNSTR) schedule.o: constraints/constraint.o section.o teacher.o
main-tester.o: schedule.o group.o
parser.o: section.o schedule.o teacher.o subject.o
emitter.o solver.o: schedule.o
group.o: section.o subject.o

clean:
	rm -f $(OBJS) $(CNSTR)
distclean: clean
	rm -f main main.exe