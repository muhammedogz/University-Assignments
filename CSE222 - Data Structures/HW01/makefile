JC = javac
R = java
SUPER = CSE222_hw01/interface_oguz/
SUB = CSE222_hw01/src_oguz/
DRIVER = Test
JFlags = 


muo:
	$(JC) $(JFlags) $(SUPER)*.java $(SUB)*.java $(DRIVER).java -d classfiles
	$(R) $(DRIVER)

javadoc:
	javadoc $(SUPER)*.java $(SUB)*.java -d javadoc

clean:
	rm javadoc/ classfiles -r


	