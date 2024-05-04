## TORALIZE
A small compiled library to redirect commands through tor using the socks4 protocol (https://www.openssh.com/txt/socks4.protocol) to create a TCP proxy across firewalls.

To run:

- Run the cmake in some directory to create toralize.so

- Add this bash script to /usr/bin to link against the .so:

```bash
#!/bin/bash

export LD_PRELOAD=/..path..to../toralize.so
${@}
unset LD_PRELOAD
```

Now you can use the shared library. Run commands with redirection in the terminal. For example:
```bash
toralize curl https://www.google.com
```

### DEPENDENCIES
- tor (see: https://www.torproject.org/)

### NOTES:
If you are having trouble with the connections i.e. the program hangs on "Connected to proxy" then you will need to restart your tor server:
```bash
/etc/init.d/tor restart
```
