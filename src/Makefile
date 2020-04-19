CC = clang++
CFLAGS = -Wall -Wextra -g -std=c++14 -c -I/usr/local/include/libcluster -I/usr/local/include/eigen3
LDFLAGS = -L/usr/local/lib/  $(LDLIBS)
LDLIBS = -lcluster
DEPS = BGMM.h model.h BayesianLinearReg.h stacker.h AnomalyDetectionSystem.h settingHelper.h IsolationForest.h stateHelper.h

%.o: %.cpp $(DEPS)
	$(CC) -o $@ $< $(CFLAGS)

testADS: testADS.o BGMM.o BayesianLinearReg.o stacker.o AnomalyDetectionSystem.o settingHelper.o IsolationForest.o stateHelper.o
	$(CC) $(LDFLAGS) -o $@ $^

ADSSettingHelper: settingHelper.o getUserSetting.o
	$(CC) $(LDFLAGS) -o $@ $^	

all: BGMM.o BayesianLinearReg.o stacker.o AnomalyDetectionSystem.o settingHelper.o
	$(CC) $(LDFLAGS) -c -o $@ $^	