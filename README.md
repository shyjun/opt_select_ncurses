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


Screenshots
--------

Multi mode screenshot

![Description of the image](https://private-user-images.githubusercontent.com/29661934/348546033-2d0331b7-e6a8-4c95-b0a2-5c8ba844a8b8.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MjA5NjQwMzksIm5iZiI6MTcyMDk2MzczOSwicGF0aCI6Ii8yOTY2MTkzNC8zNDg1NDYwMzMtMmQwMzMxYjctZTZhOC00Yzk1LWIwYTItNWM4YmE4NDRhOGI4LnBuZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNDA3MTQlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjQwNzE0VDEzMjg1OVomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPTJiZGRmYmEyMjdhNWI4Y2M4ODA5MTUzNjgzZjNkZTIzYjEyN2FjYWIwMzg1MTUzY2Y0ZDNmYjg0NWQ2Mzg1YmImWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0JmFjdG9yX2lkPTAma2V5X2lkPTAmcmVwb19pZD0wIn0.c1SRSdG6BUsza8HmaGJReauMhD_yKsq-HjyUBONnWso)

---

Single mode screenshot
![Description of the image](https://private-user-images.githubusercontent.com/29661934/348546094-4ecca177-8938-404a-a4c7-99086ba4ba59.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MjA5NjQwODQsIm5iZiI6MTcyMDk2Mzc4NCwicGF0aCI6Ii8yOTY2MTkzNC8zNDg1NDYwOTQtNGVjY2ExNzctODkzOC00MDRhLWE0YzctOTkwODZiYTRiYTU5LnBuZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNDA3MTQlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjQwNzE0VDEzMjk0NFomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPTg5ZGJhMmE4NDRkNzgzNzg1MDJhMjAxMDNkYmFhYWQwYWU1NTg1NmRhMTMwYzhkZjkwNGU4NzRlNjJhNjI2ZDcmWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0JmFjdG9yX2lkPTAma2V5X2lkPTAmcmVwb19pZD0wIn0.ot1eLkpRFN285T-kELzVm4KRPjRaBSoAyggYA6WDYMY)