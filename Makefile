# 自分のVSCode用のCode Runnerという拡張機能用のMakefile
# Code Runner側でmake run main=$fullFileName debug=no opt=noを実行するようにしている

main = main.cpp
debug = yes
opt = yes

MAIN = $(shell echo $(main) | sed -e 's/ /\\ /g')

OPTIONS = -Wno-sign-compare -Wno-char-subscripts
DEFINES =

ifneq ($(debug), no)
OPTIONS += -fdiagnostics-color=always -Wall -Wextra -Wfloat-equal -ftrapv -fstack-protector-all -fsanitize=address,undefined -fno-omit-frame-pointer -g
DEFINES += -D _GLIBCXX_DEBUG
endif
ifneq ($(opt), no)
OPTIONS += -O2
endif

.PHONY: run
run: $(SRCS)
	g++ -std=c++20 $(OPTIONS) $(DEFINES) $(MAIN) -o z.out && ./z.out
