# opt_select_ncurses
simple app to show ncurses based GUI to do a selection from multi choice


#### Syntax
----
```
opt_select_ncurses <options_file> <selected_file> [multi_select=yes]
```


#### HOW-TO
----
Write the multi options to <options_file> and pass it as first argument to `opt_select_ncurses`. 

The tool gives ncurses based UI to choose from selections, and once done, it writes the selected item to <selected_file>


#### Dependency
----
This tool depends on ncurses to create the GUI like window. Please instlal ncurses using the command

```
sudo apt-get install libncurses-dev
sudo apt-get install ncurses-dev
apt install libncurses5-dev libncursesw5-dev
```

#### Key bindings
----

| Key                   | Operation                                         |
| ---                   | ---                                               |
| `UP`                  | Previous option                                   |
| `CTRL+p`              | Previous option                                   |
| `DOWN`                | Next option                                       |
| `CTRL+n`              | Next option                                       |
| `CTRL+d`              | 4 options down                                    |
| `CTRL+u`              | 4 options up                                      |
| `PAGE_DOWN`           | 8 options down                                    |
| `PAGE_UP`             | 8 options up                                      |
| `Esc`                 | Exit the app                                      |
| `CTRL+c`              | Exit the app                                      |
| `Enter`               | Done selecting. return                            |
| `Space`               | Select the current option (in multi select mode)  |

How to Build
--------
```
make
```


How to test
--------
```
make test
```

How to integrate it with simple bash script
--------
```
#!/bin/bash

rm -fv ./options.txt
echo "Please choose an option" >> ./options.txt

echo "option1" >> ./options.txt
echo "option2" >> ./options.txt
echo "option3" >> ./options.txt
echo "option4" >> ./options.txt
echo "option5" >> ./options.txt
echo "new option6" >> ./options.txt
echo "new new option7" >> ./options.txt
echo "again new option8" >> ./options.txt
opt_select_ncurses ./options.txt ./options.txt multi_select=yes
return_value=$?
# check if opt_select_ncurses returned non-zero
if [ $return_value -ne 0 ]; then
echo "opt_select_ncurses failed with exit code: $return_value"
rm -rf $temp_filename
exit
fi

selected=`cat $temp_filename | sed 's/\,/ /g'`
rm -rf $temp_filename
echo "Selected option is"
cat options.txt
echo

```
