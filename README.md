# librenms-plugin-sourceplayercount

This is a plugin for [LibreNMS](https://www.librenms.org/). It uses the services system which allows LibreNMS to run Nagios plugins. It is able to track the number of players connected to a game server that uses the Source 
engine. It supports both alerting and performance data recording, which allows for RRD graphing support to track historical server usage.

##Compatible games:
This plugin should be compatible with any game which runs on the source engine. Below is a list of some of the most popular titles this plugin supports.
- TF2
- CS:GO / CSS
- L4D2
- HL2:DM

##Installation
Once you have cloned the repository, run the "make" command in the directory and it will produce a binary nammed "check_source_playercount". Copy this to a folder for your LibreNMS services

edit your LibreNMS config.php and add the line:

```
$config['nagios_plugins']   = "/path/to/new/folder";
```

In LibreNMS go to Services -> Add service

For the "Type" dropdown, select check_source_playercount, and in the parameters type something like:

```
27015 10 15
```

where the first number is the port, the second is the number of players to trigger a warning, and the third is the number of players to trigger a critical alert.

If you just want to use this plugin for measuring metrics rather than alerting, just set the last two values to something greater than the number of slots.
