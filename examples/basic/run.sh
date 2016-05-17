rm src/*
../../def2cpp/x86_linux64bit/logspec2cpp log_specification.def -osrc/
g++ -std=c++11  src/log4kmw_states.cpp src/log4kmw_loggers.cpp src/log4kmw_records.cpp src/log4kmw_events.cpp src/log4kmw_loggers_tests.cpp ../../src/*cpp test_runner.cpp -Isrc/ -I../../include -I"/home/tprerovs/projects/ceps/core/include" -o test_runner
