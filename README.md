# portscanner
Simple TCP port scanner

### How to compile
```bash
gcc -o portscanner portscanner.c
```

### Usage
```bash
sudo ./portscanner -a <target_address> -p <port_list>
```

The port list accepts comma and hyphen separated entries as long as there are no spaces.

Example: `-p 21-23,53,80,135-139,443,445,554,631,902,2000,3389,5900,8291`
