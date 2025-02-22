
BIN  := ./opt_select_ncurses

.PRECIOUS: $(BIN)

OUT  := out
SRCS := main.c udp_dbg_client.c
OBJS := $(addprefix $(OUT)/,$(SRCS:.c=.o))
DEPS := $(addprefix $(OUT)/,$(SRCS:.c=.d))

CFLAGS := -MD -MP

all: $(BIN)

test: test_single_opt test_multi_opt

test_single_opt:
	@echo
	@read -p "test single select opt. Press enter to start" var
	@echo "Creating options file..."
	@rm -fv ./options.txt
	@echo "Please choose an option" >> ./options.txt
	@echo "option1" >> ./options.txt
	@echo "option2" >> ./options.txt
	@echo "option3" >> ./options.txt
	@echo "option4" >> ./options.txt
	@echo "option5" >> ./options.txt
	@echo "new option6" >> ./options.txt
	@echo "new new option7" >> ./options.txt
	@echo "abcd" >> ./options.txt
	@echo "efgh" >> ./options.txt
	@echo "ijkl" >> ./options.txt
	@echo "mnop" >> ./options.txt
	@echo "qrst" >> ./options.txt
	@echo "uvw" >> ./options.txt
	@echo "xyz" >> ./options.txt
	@echo "qwer" >> ./options.txt
	@echo "asdf" >> ./options.txt
	@echo "zxcv" >> ./options.txt
	@echo "again new option8" >> ./options.txt
	$(BIN) in_file=./options.txt out_file=./options.txt udp_dbg_port=8050
	@echo "Selected option is"
	@cat options.txt
	@echo

test_multi_opt:
	@echo
	@read -p "test multi select opt. Press enter to start" var
	@echo "Creating options file..."
	@rm -fv ./options.txt
	@echo "Please choose an option" >> ./options.txt
	@echo "option1" >> ./options.txt
	@echo "option2" >> ./options.txt
	@echo "option3" >> ./options.txt
	@echo "option4" >> ./options.txt
	@echo "option5" >> ./options.txt
	@echo "new option6" >> ./options.txt
	@echo "new new option7" >> ./options.txt
	@echo "again new option8" >> ./options.txt
	$(BIN) in_file=./options.txt out_file=./options.txt multi_select=yes #udp_dbg_port=8050
	@echo "Selected option is"
	@cat options.txt
	@echo


.PHONY: all clean

$(BIN):$(OBJS)
	gcc -o $@ $^ -lncurses

$(OUT)/%.o:%.c Makefile
	@mkdir -p $(dir $@)
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OUT) $(BIN)

-include $(DEPS)
