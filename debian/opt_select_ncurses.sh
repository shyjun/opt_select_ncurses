
#!/bin/bash

# Add script directory to PATH
export PATH="$PATH:/usr/share/opt_select_ncurses/scripts"

ACD_DIR="/usr/share/opt_select_ncurses"
ACD_FILE="$ACD_DIR/acd_func.sh"

# 1. Auto-source acd_func.sh when bash starts
if [ -f "$ACD_FILE" ]; then
    . "$ACD_FILE"
else
    # acd_func missing → cd tracking won't work yet
    export OPT_ACD_MISSING=1
fi

# 2. cdvi wrapper function
cdvi() {

    # If acd_func.sh was not sourced
    if [ "$OPT_ACD_MISSING" = "1" ]; then
        echo ""
        echo "[opt_select_ncurses] cdvi requires acd_func.sh."
        echo "Install it using:"
        echo ""
        echo "   git clone https://gist.github.com/shyjun/c097624b27b934e5b2a6a38c29613024.git /tmp/acd_tmp"
        echo "   sudo cp /tmp/acd_tmp/acd_func.sh /usr/share/opt_select_ncurses/"
        echo "   rm -rf /tmp/acd_tmp"
        echo ""
        echo "Then open a new terminal."
        echo ""
        return 1
    fi

    # If acd_func.sh was successfully sourced:
    cd -- > /tmp/opt_cd_list.txt
    cdopt
    source /usr/share/opt_select_ncurses/scripts/cdloc
}
