
.DEFAULT_GOAL := all

# Compiler and flags
CC     := gcc
CFLAGS := -Wall -Wextra -Werror -MD -MP -I./inc
LDFLAGS:= -L ./
LDLIBS := -lopt_select_ncurses -lncurses

LIB := libopt_select_ncurses.a
# Output binary
APP  := ./opt_select_ncurses
TEST_APP := ./test_app

.PRECIOUS: $(APP)

# Output directory
OUT  := out

# Lib Source files
LIB_SRCS := src/udp_dbg_client.c src/opt_select_ncurses_lib.c
# Lib obj files
LIB_OBJS := $(addprefix $(OUT)/,$(LIB_SRCS:.c=.o))

# APP Source files
APP_SRCS := src/main.c
# Object files and dependency files
APP_OBJS := $(addprefix $(OUT)/,$(APP_SRCS:.c=.o))

TEST_SRCS := src/test_app.c
TEST_OBJS := $(addprefix $(OUT)/,$(TEST_SRCS:.c=.o))

DEPS := $(APP_OBJS:.o=.d) $(LIB_OBJS:.o=.d) $(TEST_OBJS:.o=.d)

# Compile source files into objects
$(OUT)/%.o: %.c Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Default target
all: $(LIB) $(APP) $(TEST_APP)

$(LIB): $(LIB_OBJS)
	ar -rcs $@ $^

# create the app
$(APP): $(APP_OBJS) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $(filter-out %.a, $^) $(LDLIBS)

# create the test_app
$(TEST_APP): $(TEST_OBJS) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $(filter-out %.a, $^) $(LDLIBS)

# Cleanup
clean:
	rm -rf $(OUT) $(APP)

# Include dependency files
-include $(DEPS)

.PHONY: all clean test test_single_opt test_multi_opt

test: test_single_opt test_multi_opt

test_single_opt:
	@echo
	@read -p "test single select opt. Press enter to start" var
	@echo "Creating options file..."
	@rm -fv ./options.txt
	@echo "Please choose an option" >> ./options.txt
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
	@echo "option1" >> ./options.txt
	@echo "option2" >> ./options.txt
	@echo "option3" >> ./options.txt
	@echo "option4" >> ./options.txt
	@echo "option5" >> ./options.txt
	@echo "new option6" >> ./options.txt
	@echo "new new option7" >> ./options.txt
	@echo "again new option8" >> ./options.txt
	$(APP) in_file=./options.txt out_file=./options.txt udp_dbg_port=8050
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
	$(APP) in_file=./options.txt out_file=./options.txt multi_select=yes #udp_dbg_port=8050
	@echo "Selected option is"
	@cat options.txt
	@echo
