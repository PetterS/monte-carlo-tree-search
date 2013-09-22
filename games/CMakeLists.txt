# Author: petter.strandmark@gmail.com (Petter Strandmark)

MACRO (CREATE_EXAMPLE NAME)
	ADD_EXECUTABLE(${NAME}
	               ${NAME}.cpp
	               ${MCTS_HEADERS})
	MESSAGE("-- Adding game: " ${NAME})
ENDMACRO (CREATE_EXAMPLE)

#CREATE_EXAMPLE(chess)
CREATE_EXAMPLE(connect_four)
CREATE_EXAMPLE(nim)

IF (${USE_CINDER})
	MACRO (CREATE_CINDER_EXAMPLE NAME)
		ADD_EXECUTABLE(${NAME}
		               WIN32
		               ${NAME}.cpp
		               ${MCTS_HEADERS})
		TARGET_LINK_LIBRARIES(${NAME} ${CINDER_LIB})
		MESSAGE("-- Adding Cinder game: " ${NAME})
	ENDMACRO()
	CREATE_CINDER_EXAMPLE(go)
ENDIF()
