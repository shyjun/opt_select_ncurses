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
./opt_select_ncurses $temp_filename $temp_filename multi_select=yes
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
--------
```
#!/bin/bash

# Generate a random temp_filename
temp_filename=$(mktemp)

# Caption of the window
echo "Please choose the branch" > $temp_filename

git branch | sed 's/^\*/ /g' >> $temp_filename

echo

opt_select_ncurses/opt_select_ncurses $temp_filename $temp_filename

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
![Description of the image](https://private-user-images.githubusercontent.com/29661934/351754997-2fef2043-afda-4aa9-9977-739434cb2fe6.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MjE4MzE3MjEsIm5iZiI6MTcyMTgzMTQyMSwicGF0aCI6Ii8yOTY2MTkzNC8zNTE3NTQ5OTctMmZlZjIwNDMtYWZkYS00YWE5LTk5NzctNzM5NDM0Y2IyZmU2LnBuZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNDA3MjQlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjQwNzI0VDE0MzAyMVomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPWI4M2ZmYjE3NjIzOTUzNjQwNjEzMTU1ZDJlOTAwZTc0ZThjMmJhNzRlNTNlZDYzODIzODFmNmU4YjMxNTA2YjImWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0JmFjdG9yX2lkPTAma2V5X2lkPTAmcmVwb19pZD0wIn0.6NnoIwP0kWBUrrqu-SoLhTZJEsprTRA4xmoTgWLGKNc)


Screenshots
--------

Multi mode screenshot

![Description of the image](https://private-user-images.githubusercontent.com/29661934/348546033-2d0331b7-e6a8-4c95-b0a2-5c8ba844a8b8.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MjE4MzA3MTEsIm5iZiI6MTcyMTgzMDQxMSwicGF0aCI6Ii8yOTY2MTkzNC8zNDg1NDYwMzMtMmQwMzMxYjctZTZhOC00Yzk1LWIwYTItNWM4YmE4NDRhOGI4LnBuZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNDA3MjQlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjQwNzI0VDE0MTMzMVomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPTBhZWRjNzlmOGIwMjBkZmI4MThmYzM2OTdhYjUxY2QwNGEyZjI2NDVmOGE3Y2IwOTRiNGU5MzEwNGE1MDkwODImWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0JmFjdG9yX2lkPTAma2V5X2lkPTAmcmVwb19pZD0wIn0.Ye1WYSjW_zPrZlO6cOlppYO3K3PENW1EiNJc2DZDEBs)

---

Single mode screenshot
![Description of the image](https://private-user-images.githubusercontent.com/29661934/348546094-4ecca177-8938-404a-a4c7-99086ba4ba59.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MjE4MzA2NjYsIm5iZiI6MTcyMTgzMDM2NiwicGF0aCI6Ii8yOTY2MTkzNC8zNDg1NDYwOTQtNGVjY2ExNzctODkzOC00MDRhLWE0YzctOTkwODZiYTRiYTU5LnBuZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNDA3MjQlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjQwNzI0VDE0MTI0NlomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPTJmMGMwNjhlZGI5OGQ2YzI3OGFlZDQzZjY2ZGRlMGUxYzYxMDQ1ZTM1MGNjYmQ0ZGJjMjg1OWY3Mjk2MTc5ZjgmWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0JmFjdG9yX2lkPTAma2V5X2lkPTAmcmVwb19pZD0wIn0.LnKSwnVsGbk18xrDfXOCgih54Xq49bjlCK3j_5rRlX4)
