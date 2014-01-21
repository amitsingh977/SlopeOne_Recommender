SlopeOne_Recommender
====================

This program takes as parameters an input .txt file and a user number. It executes the SlopeOne algorithm in order to find the predicted ratings of users on certain items. The program is divided in a main section used primarily for variables creation, memory allocation when needed and for calling functions. Function header_checker extracts the users and items, getRatings is used to obtain all ratings in the file and save them to a matrix. Finally, the calcs function calculates averages, frequencies, and* the factors of the final formula before printing out all results. If a number of highest predicted ratings is given, then it shows the list, otherwise it prints all ratings, both predicted and not predicted, for the given user.
