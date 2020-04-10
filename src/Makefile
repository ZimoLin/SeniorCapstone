CC = clang++
CFLAGS = -Wall -Wextra -g -std=c++14 -c -I/usr/local/include/libcluster -I/usr/local/include/eigen3
LDFLAGS = -L/usr/local/lib/  $(LDLIBS)
LDLIBS = -lcluster
DEPS = BGMM.h model.h BayesianLinearReg.h stacker.h AnomalyDetectionSystem.h setting_helper.h IsolationForest.h stateHelper.h

%.o: %.cpp $(DEPS)
	$(CC) -o $@ $< $(CFLAGS)

test: test.o BGMM.o
	$(CC) $(LDFLAGS) -o $@ $^

test1: test1.o BGMM.o 
	$(CC) $(LDFLAGS) -o $@ $^

testStacker: testStacker.o BGMM.o BayesianLinearReg.o stacker.o
	$(CC) $(LDFLAGS) -o $@ $^

testADS: testADS.o BGMM.o BayesianLinearReg.o stacker.o AnomalyDetectionSystem.o setting_helper.o IsolationForest.o stateHelper.o
	$(CC) $(LDFLAGS) -o $@ $^

ADSSettingHelper: setting_helper.o getUserSetting.o
	$(CC) $(LDFLAGS) -o $@ $^	

all: BGMM.o BayesianLinearReg.o stacker.o AnomalyDetectionSystem.o setting_helper.o
	$(CC) $(LDFLAGS) -c -o $@ $^	

IFTest: IsolationForest.o IFTest.o
	$(CC) $(LDFLAGS) -o $@ $^