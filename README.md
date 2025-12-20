# opt_select_ncurses

Ncurses-based GUI selector utility

And a simple app to show ncurses-based UI to select from multiple choices.

**Author:** Shyju N (n.shyju@gmail.com)


#### Syntax
---
```
opt_select_ncurses [in_file=<input_file>] [out_file=<output_file>] [multi_select=yes] [default=<value>] [from_pipe=yes/no] [udp_dbg_port=<udp_dbg_server_port>] [-h  for help]
```

| Option           | feature                                                                                     |
| -----------------|---------------------------------------------------------------------------------------------|
| `in_file`        | The input options are read from this file                                                   |
| `out_file`       | The selected data will be written to this file                                              |
| `multi_select`   | Whether its multi-select                                                                    |
| `default`        | Default selected value/idx                                                                  |
| `from_pipe`      | If the input is from a pipe                                                                 |
| `udp_dbg_port`   | UDP port where dbg info to be sent out. Use `nc -k -l -u -p <PORT>` to listen to debug msgs |
| `-h`             | Show help                                                        

## Installation
---

#### 1. One-line install (recommended)

Just copy & paste:

```
wget -qO- https://shyjun.github.io/opt_select_ncurses-apt/install.sh | sudo bash
```

This will:

* Add the APT repository

* Update package lists

* Install the latest version of opt-select-ncurses

* Set up shell scripts and PATH

#### 2. Add APT repository manually

If you prefer adding the repo manually:

```
echo "deb [trusted=yes] https://shyjun.github.io/opt_select_ncurses-apt/apt ./" \
    | sudo tee /etc/apt/sources.list.d/opt-select-ncurses.list
```

Update:

```
sudo apt update
```


Install:

```
sudo apt install opt-select-ncurses
```

#### 3. Install directly from .deb file

Download the .deb from:

```
https://shyjun.github.io/opt_select_ncurses-apt/apt/opt_select_ncurses_1.0-1_amd64.deb
```

Then install:

```
sudo dpkg -i opt_select_ncurses_1.0-1_amd64.deb
sudo apt --fix-broken install
```

#### 4. Verify installation

Check the binary:

```
which opt_select_ncurses
```

Check version/help:

```
opt_select_ncurses --help
```

#### 5. Installed File Locations

After installation, files are placed in standard Linux paths:

Binary
```
/usr/bin/opt_select_ncurses
```

Library
```
/usr/lib/libopt_select_ncurses.a
```

Header
```
/usr/include/opt_select_ncurses/opt_select_ncurses_lib.h
```

##### Scripts (added to PATH automatically)

These helper scripts are globally available:

```
cdloc
cdopt
fops
gcb
grepopt
gsvi
llopt
output_sel
vif
```

Actual location of scripts:

```
/usr/share/opt_select_ncurses/scripts/
```

Installation location of full Source Tree (installed for development/rebuild)
```
/usr/share/opt_select_ncurses/
├── inc
│   └── opt_select_ncurses_lib.h
├── libopt_select_ncurses.a
├── Makefile
├── options.txt
├── README.md
├── scripts
│   ├── cdloc
│   ├── cdopt
│   ├── fops
│   ├── gcb
│   ├── grepopt
│   ├── gsvi
│   ├── llopt
│   ├── output_sel.sh
│   └── vif
└── src
    ├── main.c
    ├── opt_select_ncurses_lib.c
    ├── test_app.c
    └── udp_dbg_client.c
```


#### 6. Rebuilding (optional)

You can rebuild from the installed source tree:

```
cd /usr/share/opt_select_ncurses
make
sudo make install
```


#### HOW-TO

---

Write the multi options to <input_file> and pass it as first argument to `opt_select_ncurses`. First line should be the caption, something like: "Please select from below" or so, (which is excluded from the list of items)


The tool gives ncurses based UI to choose from selections, and once done, it writes the selected item to <output_file>


Other option is to pass the input from pipe like this:

```
echo -e  "select\none\ntwo\nthree\nfour\nfive\nsix" | opt_select_ncurses from_pipe=yes default=3 multi_select=yes
```

#### Dependency
---

This tool depends on ncurses to create the GUI like window. Please instlal ncurses using the command

```
sudo apt-get install libncurses-dev
sudo apt-get install ncurses-dev
sudo apt install libncurses5-dev libncursesw5-dev
```

#### Key bindings
---

Normal Mode:


| Key         | Operation                                        |
| ----------- | ------------------------------------------------ |
| `UP`        | Previous option                                  |
| `CTRL+p`    | Previous option                                  |
| `DOWN`      | Next option                                      |
| `CTRL+n`    | Next option                                      |
| `CTRL+d`    | 4 options down                                   |
| `CTRL+u`    | 4 options up                                     |
| `PAGE_DOWN` | 8 options down                                   |
| `PAGE_UP`   | 8 options up                                     |
| `Esc`       | Exit the app                                     |
| `CTRL+c`    | Exit the app                                     |
| `Enter`     | Done selecting. return                           |
| `Space`     | Select the current option (in multi select mode) |



Grep Mode(searching):


| Key         | Operation                                        |
| ----------- | ------------------------------------------------ |
| `CTRL+p`    | Previous option                                  |
| `CTRL+n`    | Next option                                      |
| `Esc`       | Exit Grep mode and switch to normal mode         |
| `CTRL+c`    | Exit the app                                     |
| `Enter`     | Done selecting. return                           |
| `Space`     | Select the current option (in multi select mode) |


How to Build
------------
```
make
```


How to test
-----------
```
make test
```

How to integrate it with simple bash script
-------------------------------------------
```
#!/bin/bash

temp_filename=$(mktemp)
echo "Please choose an option" >> $temp_filename

echo "option1" >> $temp_filename
echo "option2" >> $temp_filename
echo "option3" >> $temp_filename
echo "option4" >> $temp_filename
echo "option5" >> $temp_filename
echo "new_option6" >> $temp_filename
echo "new_new_option7" >> $temp_filename
echo "again_new_option8" >> $temp_filename
./opt_select_ncurses in_file=$temp_filename out_file=$temp_filename multi_select=yes
return_value=$?
# check if opt_select_ncurses returned non-zero
if [ $return_value -ne 0 ]; then
    echo "opt_select_ncurses returned exit code: $return_value"
    rm -rf $temp_filename
    exit
fi

selected=`cat $temp_filename | sed 's/\,/ /g'`
rm -rf $temp_filename
echo "Selected option is"
echo $selected
echo
```

How to integrate it with simple bash script, which selects the git branch
-------------------------------------------------------------------------
```
#!/bin/bash

# Generate a random temp_filename
temp_filename=$(mktemp)

# Caption of the window
echo "Please choose the branch" > $temp_filename

git branch | sed 's/^\*/ /g' >> $temp_filename

echo

opt_select_ncurses/opt_select_ncurses in_file=$temp_filename out_file=$temp_filename

return_value=$?
# check if opt_select_ncurses returned non-zero
if [ $return_value -ne 0 ]; then
    echo "opt_select_ncurses failed with exit code: $return_value"
    rm -rf $temp_filename
    exit
fi

actual_branch=`cat $temp_filename`
rm -rf $temp_filename

echo "$0: branch: $actual_branch"

git checkout $actual_branch


```
Screenshot of git branch selector

![git_branch_selector](https://github.com/user-attachments/assets/6d68ac6d-d61c-4952-b508-67d0fc2979c5)

Screenshots
--------

Multi mode screenshot

![multi_mode](https://github.com/user-attachments/assets/e5a15640-e89c-4e10-b7bf-ac9a2b03c263)

---

Single mode screenshot

![single_mode](https://github.com/user-attachments/assets/4fc63900-5c9d-410c-8fe8-7a6b45b457bb)
