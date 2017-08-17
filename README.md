# pchat-server

## Intro

pchat-server is a simple chatroom's server based on Linux epoll with MySQL.

## Install

### Install dependencies

pchat-server depends on `libportal`, `mysql++` and `jsoncpp`. You can get these dependencies by:

``` shell
git clone https://github.com/hepangda/libportal.git
cd libportal
make
pacman -S mysql++
pacman -S jsoncpp
```

### Pre-install guide

#### Create Mysql databases

Execute `create database pchat;` and copy MySQL command in `doc/InitMySQL.sql`, it'll create database automaticly.

#### Configure `database_connstr.h`

`database_connstr.h` is not been add to source code but it needed by `make`, so you need add it to source code. Please create a text file named `database_connstr.h` like this:

``` c++
#define CONNECT_STR "Database Name", "MySQL Server IP", \
                    "MySQL Username", "MySQL Password" 
```

### Install

Ensure you're finished all of prerequisites before. Then you can get the binrary executable server just by enter `make` in terminal.

## Execute

After you make it, you can run it by `./pchat-server &`, and server will work.