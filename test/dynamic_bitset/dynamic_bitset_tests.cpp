#include <iostream>
#include <type_traits>
#include <tuple>
#include <cstdint>
#include <memory>
#include <iterator>
#include <limits>
#include <cstdlib>
#include <time.h>
#include <sstream>
#include <vector>

#include "log4ceps_serialization.hpp"
#include "log4ceps_state.hpp"
#include "log4ceps_record.hpp"
#include "log4ceps_ringbuffer.hpp"
#include "log4ceps_logger.hpp"
#include "log4ceps_dynamic_bitset.hpp"
#include <assert.h>

int main(int argc, char** argv){
 log4ceps::Dynamic_bitset bitset1(8);
 log4ceps::Dynamic_bitset bitset2(9);
 log4ceps::Dynamic_bitset bitset3(17);
 log4ceps::Dynamic_bitset bitset4(32);

 assert(1 == bitset1.allocated_memory_size());
 assert(2 == bitset2.allocated_memory_size());
 assert(3 == bitset3.allocated_memory_size());
 assert(4 == bitset4.allocated_memory_size());

 assert(bitset1[0] == 0);
 assert(bitset1[1] == 0);
 assert(bitset1[2] == 0);
 assert(bitset1[3] == 0);
 assert(bitset1[4] == 0);
 assert(bitset1[5] == 0);
 assert(bitset1[6] == 0);
 assert(bitset1[7] == 0);

 bitset1.set(0);
 bitset1.set(2);
 bitset1.set(5);

 assert(bitset1[0] == 1);
 assert(bitset1[1] == 0);
 assert(bitset1[2] == 1);
 assert(bitset1[3] == 0);
 assert(bitset1[4] == 0);
 assert(bitset1[5] == 1);
 assert(bitset1[6] == 0);
 assert(bitset1[7] == 0);

 bitset1.reset(0);
 bitset1.reset(2);
 bitset1.reset(5);
 bitset1.reset(1);

 assert(bitset1[0] == 0);
 assert(bitset1[1] == 0);
 assert(bitset1[2] == 0);
 assert(bitset1[3] == 0);
 assert(bitset1[4] == 0);
 assert(bitset1[5] == 0);
 assert(bitset1[6] == 0);
 assert(bitset1[7] == 0);

 bitset1.set(std::vector<int>{0,5,2});

 std::cout << "bitset1=" <<bitset1 << std::endl;

 assert(bitset1[0] == 1);
 assert(bitset1[1] == 0);
 assert(bitset1[2] == 1);
 assert(bitset1[3] == 0);
 assert(bitset1[4] == 0);
 assert(bitset1[5] == 1);
 assert(bitset1[6] == 0);
 assert(bitset1[7] == 0);


 assert(bitset4[0] == 0);
 assert(bitset4[1] == 0);
 assert(bitset4[2] == 0);
 assert(bitset4[3] == 0);
 assert(bitset4[4] == 0);
 assert(bitset4[5] == 0);
 assert(bitset4[6] == 0);
 assert(bitset4[7] == 0);

 assert(bitset4[8] == 0);
 assert(bitset4[9] == 0);
 assert(bitset4[10] == 0);
 assert(bitset4[11] == 0);
 assert(bitset4[12] == 0);
 assert(bitset4[13] == 0);
 assert(bitset4[14] == 0);
 assert(bitset4[15] == 0);

 assert(bitset4[16] == 0);
 assert(bitset4[17] == 0);
 assert(bitset4[18] == 0);
 assert(bitset4[19] == 0);
 assert(bitset4[20] == 0);
 assert(bitset4[21] == 0);
 assert(bitset4[22] == 0);
 assert(bitset4[23] == 0);

 assert(bitset4[24] == 0);
 assert(bitset4[25] == 0);
 assert(bitset4[26] == 0);
 assert(bitset4[27] == 0);
 assert(bitset4[28] == 0);
 assert(bitset4[29] == 0);
 assert(bitset4[30] == 0);
 assert(bitset4[31] == 0);


 bitset4.set(0);
 bitset4.set(2);
 bitset4.set(5);

 bitset4.set(8);
 bitset4.set(9);
 bitset4.set(30);


 assert(bitset4.get(0) == 1);
 assert(bitset4[1] == 0);
 assert(bitset4[2] == 1);
 assert(bitset4[3] == 0);
 assert(bitset4[4] == 0);
 assert(bitset4.get(5));
 assert(!bitset4.get(6));
 assert(bitset4[7] == 0);

 assert(bitset4[8] == 1);
 assert(bitset4[9] == 1);
 assert(bitset4[10] == 0);
 assert(bitset4[11] == 0);
 assert(bitset4[12] == 0);
 assert(bitset4[13] == 0);
 assert(bitset4[14] == 0);
 assert(bitset4[15] == 0);

 assert(bitset4[16] == 0);
 assert(bitset4[17] == 0);
 assert(bitset4[18] == 0);
 assert(bitset4[19] == 0);
 assert(bitset4[20] == 0);
 assert(bitset4[21] == 0);
 assert(bitset4[22] == 0);
 assert(bitset4[23] == 0);

 assert(bitset4[24] == 0);
 assert(bitset4[25] == 0);
 assert(bitset4[26] == 0);
 assert(bitset4[27] == 0);
 assert(bitset4[28] == 0);
 assert(bitset4[29] == 0);
 assert(bitset4[30] == 1);
 assert(bitset4[31] == 0);

 std::cout << "bitset4=" <<bitset4 << std::endl;

 char buffer1[32] = {};

 log4ceps::serialize_value(bitset4,buffer1,32,true ,log4ceps::nothrow_exception_policy());

 bitset4.clear();

 std::cout << "bitset4=" <<bitset4 << std::endl;

 log4ceps::deserialize_value(bitset4,buffer1,32);

 std::cout << "bitset4=" <<bitset4 << std::endl;

 assert(bitset4.get(0) == 1);
 assert(bitset4[1] == 0);
 assert(bitset4[2] == 1);
 assert(bitset4[3] == 0);
 assert(bitset4[4] == 0);
 assert(bitset4.get(5));
 assert(!bitset4.get(6));
 assert(bitset4[7] == 0);

 assert(bitset4[8] == 1);
 assert(bitset4[9] == 1);
 assert(bitset4[10] == 0);
 assert(bitset4[11] == 0);
 assert(bitset4[12] == 0);
 assert(bitset4[13] == 0);
 assert(bitset4[14] == 0);
 assert(bitset4[15] == 0);

 assert(bitset4[16] == 0);
 assert(bitset4[17] == 0);
 assert(bitset4[18] == 0);
 assert(bitset4[19] == 0);
 assert(bitset4[20] == 0);
 assert(bitset4[21] == 0);
 assert(bitset4[22] == 0);
 assert(bitset4[23] == 0);

 assert(bitset4[24] == 0);
 assert(bitset4[25] == 0);
 assert(bitset4[26] == 0);
 assert(bitset4[27] == 0);
 assert(bitset4[28] == 0);
 assert(bitset4[29] == 0);
 assert(bitset4[30] == 1);
 assert(bitset4[31] == 0);


 return 0;
}

