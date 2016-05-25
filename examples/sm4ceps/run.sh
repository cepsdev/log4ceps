../../def2cpp/x86_linux64bit/logspec2cpp log_specification.def -osrc_gen/
g++ -std=c++11  src_gen/log4kmw_states.cpp src_gen/log4kmw_loggers.cpp src_gen/log4kmw_records.cpp src_gen/log4kmw_events.cpp src_gen/log4kmw_loggers_tests.cpp ../../src/*cpp test_runner.cpp -Isrc_gen/ -I../../include -I"/home/tprerovs/projects/ceps/core/include" -o test_runner
