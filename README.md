# ProgPrincFinal
Final project for Programming Principles course
Created by Braden Burriss, Ethan Harness, and Harper Hill


To use this program you will need to have “fileTransferServer.exe” on your server and “fileTransferClient.exe” on your client machine.  First start the program on your server and then launch the program on your client, inputting the server’s IP address to connect to it.  Once connected you will be presented with a listing of the server’s base directory and a menu with six options on it.  To use an option you type the number next to it into the console and hit enter.  After executing the command the client will present the user with the same menu again, allowing them to continue navigating through the server or performing whichever action they would like to take.

1: Move – This option allows you to move into any of the folders listed in the directory by typing the number of the folder shown.  You can also enter a value of -1 to go back a directory.

2: Scan contents of a file for information – This option provides you with two more choices, to Scan Exactly or Scan Roughly.  Scan Exactly asks for a for a word and then searches a text file for that word, telling you if the file contains it.  Scan Roughly asks for a set of characters and how many of them need to match in order and scans a text file for matches.

3: Create a file – This option asks for the filepath to a file on the client and pushes that file to the server.

4: Pull a file – This option allows you to select a file on the server and pull it to the client.

5: Display current Directory – This option refreshes the display of the current directory, allowing the user to see if any updates have been made.

6: Exit – This function allows you to exit the interface and terminate the connection between the client and the server. 
