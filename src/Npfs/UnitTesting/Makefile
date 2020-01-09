
INC = -I ../.. -I../../include -I../../Interface

HelperClasses +=  ../../Interface/PrintfToStream.cpp ../../Interface/ByteStreamDecorator.cpp ../../Interface/ByteStream.cpp
HelperClasses +=  ../../Interface/MemoryManager.cpp HeapMemory.cpp

TestedClasses += ../MsgSerDes.cpp ../MsgBufferSerDes.cpp

TestList += MsgSerDes-Test.cpp


TestedClasses += ../Connection.cpp ../Messages/Rmessage.cpp ../Messages/Tmessage.cpp ../Messages/Message.cpp
TestedClasses += ../Messages/Tversion.cpp ../Messages/Tauth.cpp ../Messages/Tflush.cpp ../Messages/Tattach.cpp ../Messages/Twalk.cpp 
TestedClasses += ../Messages/Topen.cpp ../Messages/Tcreate.cpp ../Messages/Tread.cpp ../Messages/Twrite.cpp ../Messages/Tclunk.cpp 
TestedClasses += ../Messages/Tremove.cpp ../Messages/Tstat.cpp ../Messages/Twstat.cpp ../Messages/Rerror.cpp 
TestedClasses += ../Messages/Rversion.cpp ../Messages/Rauth.cpp ../Messages/Rattach.cpp ../Messages/Rflush.cpp ../Messages/Rwalk.cpp
TestedClasses += ../Messages/Ropen.cpp ../Messages/Rcreate.cpp ../Messages/Rread.cpp ../Messages/Rwrite.cpp ../Messages/Rclunk.cpp
TestedClasses += ../Messages/Rremove.cpp ../Messages/Rstat.cpp ../Messages/Rwstat.cpp
TestedClasses += ../NpStr.cpp ../NpStat.cpp ../NpQid.cpp ../NpUser.cpp ../NpfsNew.cpp

TestList += Connection-Test.cpp


TestedClasses += ../Resources/Resource.cpp ../Resources/AdmUsersFile.cpp ../Resources/AdmUsersEntry.cpp ../Resources/AdmGroup.cpp
TestedClasses += ../Resources/AdmGroupMember.cpp ../Resources/Directory.cpp ../Resources/OpenIoState.cpp ../Resources/BlockingOpState.cpp
TestedClasses += ../NpfsConstants.cpp

TestList += AdmUsers-Test.cpp

.PHONY: all clean

all: unit-test

Sources = $(HelperClasses) $(TestedClasses) $(TestList) UnitTest.cpp
Headers = $(HelperClasses:%.cpp=%.h) $(TestedClasses:%.cpp=%.h)

unit-test: $(Sources) | $(Headers)
	g++ -O -g $(INC) -o $(@) $(^)
	
test: unit-test
	-./$(<)
	@echo -e "\nexit result = $$?"

clean:
	-rm -rf unit-test
	