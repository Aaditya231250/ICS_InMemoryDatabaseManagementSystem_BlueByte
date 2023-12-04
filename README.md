
# **BLUEDB** :fox_face:
## In-Memory Database Management System

This project is a simple in-memory database implemented in C, featuring nested dictionaries and cJSON for JSON parsing. The program allows you to perform various database operations, such as adding collections, inserting data, querying data, updating data, sorting specific data, saving the database to a file, and gracefully exiting.

## Table of Contents

- [Introduction](#introduction)
- [Dependencies](#dependencies)
- [Compilation](#compilation)
- [Usage](#usage)
- [Cross-Platform Compatibility](#cross-platform-compatibility)



## Introduction

BLUEDB :fox_face: is designed to provide a simple, lightweight, and flexible in-memory data storage solution. It leverages the cJSON library for efficient JSON parsing and handling.

## Dependencies

The primary external dependency for this project is the cJSON library. You can find the cJSON library at [https://github.com/DaveGamble/cJSON](https://github.com/DaveGamble/cJSON).

A copy of that cJSON library has been included in the form of "cJSON1.h" and "cJSON.h" in the repository.
Put them in the same folder as the runnable file.
**NOTE: USE THIS (.H) FILES TO RUN THE CODE ELSE YOU MIGHT RUN INTO AN ERROR....**


## Compilation

To compile the program, you can use a C compiler such as `gcc`. Here's an example command:

```bash
if ($?) { gcc main.c -o main } ; if ($?) { .\main }
```


## Usage

* Run the compiled executable in your terminal or command prompt.
* The program will guide you through various options for interacting with the in-memory database.
* Perform operations such as adding collections, inserting data, querying data, updating data, filtering data, saving the database to a file, and exiting gracefully.
* A sample database has been attached in the repository.
<br/>_**Note : Use only a JSON Databse file.**_
*<br/>The hierarchy of the Database should be followed as:

 1) Corresponding to it, collections can be made. These collections are at present in the form of Key-Value Pairs and can contain a value in the from of these datatypes:
    1) Integer
    2) Double
    3) String
    4) Dictionary
 2) A nested dictionary and its efficient reading is an important feature.
 3) This dictionary can contain Key-Value Pairs, and corresponding to that, you can create a dictionary again or else simply values with other three data types can be assigned to the Keys.




## Cross-Platform Compatibility

The program is designed to be cross-platform. It utilizes platform-specific code for handling screen clearing and sleep functions. This ensures a consistent user experience across different operating systems.



