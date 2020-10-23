# NameBot

A Telegram user bot which updates your name automatically.

## Build Instructions

### Requirements

* JSON-C 0.15
* TDLib JSON
* GCC / GNU Make / GLibc

### Steps

1. Obtain your API ID and API Hash from [my.telegram.org][https://my.telegram.org].
2. Create a version name like.
3. Make
```shell
make CFLAGS="-DAPI_ID=<Your API ID without quotes> -DAPI_HASH=<Your API Hash without quotes> -DVERSION=<Your Version without quotes>"

# For example
make CFLAGS="-DAPI_ID=1 -DAPI_HASH=114514aaa -DVERSION=1"
```
4. Install: `sudo make install`

## Create a mod to generate name

NameBot cannot guess your dynamic name updates, so you will need a mod (in `.so`) to generate the name.

Just use the header `client/dynmodule_client.h` to create your mod. Make sure you meet the following requirements:

* The arguments to functions are all pointers to char array. The arrays are NULL at first. **Make sure to initialize (allocate) on heap.**
* Both first name and last name have a 64-characters limit.
* Return 0 to indicate success.
* First name must be supplied, while last name is optional.

Then you are ready to compile your mod.

## Usage

After doing all the preparations, you are ready to run. General configuration:

```shell
/path/to/namebot -p <data dir, will be created> \ # Can also be replaced by environment variable DATA_DIR
				-n <path/to/mod.so> \ # Can also be replaced by environment variable NAME_MOD
				-d \ # Daemon mode. Will return an error if the user had not logon. Optional.
				-c <sec> # Continuous mode. Will update the name periodically, based your given intervals (in seconds). Optional.
```

# License

GPL v2 only.
