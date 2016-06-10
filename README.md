# nagios-plugin-sourceplayercount

This is a plugin for [Nagios](https://www.nagios.org/). It is able to track the number of players connected to a game server that uses the Source engine. It supports both alerting and performance data recording, which allows you to combine this with an RRD tool such as nagiosgraph to track historical server usage.

##Compatible games:
This plugin should be compatible with any game which runs on the source engine. Below is a list of some of the most popular titles this plugin supports.
- TF2
- CS:GO / CSS
- L4D2
- HL2:DM

##Installation
Once you have cloned the repository, run the "make" command in the directory and it will produce a binary nammed "check_source_playercount". Copy this to your nagios plugins folder (on FreeBSD this is /usr/local/libexec/nagios/).

Next, you need to locate the file that contains your command definitions, by default this is usually "commands.cfg". In here add an entry such as below:

```
# Check number of players on Source Engine servers
define command {
    command_name source_players
    command_line $USER1$/check_source_playercount $ARG1$ $ARG2$ $ARG3$ $ARG4$
}
```

Finally, add the following service block to your host configuration, and adjust it to your preferences:

```
# Check to see number of players on the Source server
define service {
        use                     remote-service
        host_name               server.example.com
        service_description     Source Players
        check_command           source_players!server.example.com!27015!10!15
}
```

In the above example, Nagios will begin warning when 10 players are on the server, and will begin sending critical alerts when 15 players are on the server.

###Optional

You may wish to increase the rate of checking for the service. Below is a template for checking every minute, and notifying every 15 minutes that a warning or critical condition exists:

```
define service {
        name                                    source-service
        use                                     generic-service
        max_check_attempts                      1
        normal_check_interval                   1
        retry_check_interval                    15
        notification_interval                   15
        register                                0
}
```
