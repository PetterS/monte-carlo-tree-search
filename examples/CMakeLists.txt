# Author: petter.strandmark@gmail.com (Petter Strandmark)

MACRO (CREATE_EXAMPLE NAME)
  ADD_EXECUTABLE(${NAME}
                 ${NAME}.cpp
                 ${MCTS_HEADERS})
ENDMACRO (CREATE_EXAMPLE)

CREATE_EXAMPLE(connect_four)
