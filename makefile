CC = clang++
DEBUG_FLAGS = -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer
CFLAGS = -std=c++17 -Wall -Wextra ${DEBUG_FLAGS}

SOURCE_FILES := gameboard.cpp
OUT := gameboard.out

build: 
	${CC} ${CFLAGS} ${SOURCE_FILES} -o ${OUT}

build_nosan:
	$(CC) -g -Wall -Wextra -Wpedantic $(SOURCE_FILES) -o $(OUT)

run_san: build
	./${OUT}

clean:
	rm -f *.out
	



