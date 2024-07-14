
BIN  := ./opt_select_ncurses

OUT  := out
SRCS := main.c
OBJS := $(addprefix $(OUT)/,$(SRCS:.c=.o))
DEPS := $(addprefix $(OUT)/,$(SRCS:.c=.d))

CFLAGS := -MD -MP

all: $(BIN)

.PHONY: all clean

$(BIN):$(OBJS)
	gcc -o $@ $^ -lncurses

$(OUT)/%.o:%.c Makefile
	@echo "updated=$?"
	@mkdir -p $(dir $@)
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OUT) $(BIN)

-include $(DEPS)
