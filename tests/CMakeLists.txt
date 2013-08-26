# Author: petter.strandmark@gmail.com (Petter Strandmark)

MACRO (CREATE_TEST NAME)
	ADD_EXECUTABLE(test_${NAME} 
	               test_${NAME}.cpp
	               ${MCTS_HEADERS})

	ADD_TEST(NAME ${NAME}
	         COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_${NAME})
ENDMACRO (CREATE_TEST)

CREATE_TEST(mcts)
