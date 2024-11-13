#include "../main/lib/input.hpp"
#include <catch2/catch_all.hpp>

SCENARIO("Ring Buffer"){

  position *inputList = (position*)calloc(10,3*sizeof(uint16_t));
  for ( uint16_t i = 0; i<10; i++){
    inputList[i]=position{i,i,i};
  }
  r_buffer inputs = InputBuffer{10,0,inputList};
  GIVEN("A length of 10"){

    auto last = inputs.last();
    REQUIRE( last == 9 );
    for ( uint16_t i = 0; i <= last; i++){
      INFO("i: " << i);
      inputs.current = i;
      REQUIRE( i == inputs.current );
      inputs.setVal(position{i,i,i});
    }
    
    // stuff in a when block, does not have side effects. the next call in the require will not affect the state of the inputs object outside the block
    WHEN("Its at the last element, It wraps around on next()"){
      inputs.current = 9;
      REQUIRE( inputs.last() == 8 );
      REQUIRE( inputs.next() == 0 );
    }
    WHEN("Its at the first element, It wraps around on prev()"){
      inputs.current=0;
      REQUIRE( inputs.last() == 9 );
      REQUIRE( inputs.prev() == 9 );
    }
    inputs.current = 0;
    last = inputs.last();
    for ( uint16_t i = 0; i <= last; i++) {
      position val = *inputs.getVal();
      INFO("i: " << i  );
      REQUIRE( val.x == i );
      REQUIRE( val.y == i );
      REQUIRE( val.z == i );
      if ( inputs.current == last ) {
        break;
      }
      inputs.next();
    }
  }
};
