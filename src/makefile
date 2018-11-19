CXXFLAGS = -g -O2 -pipe -std=c++14 -Wall
CNSTR = constraints/distinctperday.o \
	constraints/evendismissal.o \
	constraints/nonsimultaneous.o \
	constraints/reqfirstsubject.o \
	constraints/subjectgaps.o \
	constraints/subjecttime.o \
	constraints/teachertime.o
OBJS = group.o schedule.o section.o subject.o teacher.o constraints/constraint.o

main: main.o $(OBJS) $(CNSTR)
	g++ $(CXXFLAGS) -o main main.o $(OBJS) $(CNSTR)

tester: main-tester.o $(OBJS) $(CNSTR)
	g++ $(CXXFLAGS) -o tester main-tester.o $(OBJS) $(CNSTR)

$(CNSTR) schedule.o: constraints/constraint.o section.o
main.o main-tester.o: schedule.o group.o
group.o: section.o subject.o

clean:
	rm -f $(OBJS) $(CNSTR)
distclean: clean
	rm -f main main.exe