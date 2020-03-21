# fierce_c
fierce_c is an incomplete analogue of [fierce](https://github.com/mschwager/fierce) written in C
(a DNS reconnaissance tool for locating non-contiguous IP space).

# Download
Check the `bin` folder. There are precompiled versions for Windows, Linux and MacOS.

# Usage
```text
Syntax: dns [options]
OPTIONS:
        -help:    print help
        -host:    hostname
        -dns:     dns server ip (1.1.1.1 by default)
        -file:    file path with prefixes
        -threads: number of threads
        -timeout: timeout for each request

EXAMPLE:
        dns -host example.ru
        dns -host example.ru -dns 8.8.8.8 -file dict/5000.txt -threads 8 -timeout 0.1
```
##### Output example:
```text
$ ./dns -host ifmo.ru      (Linux | MacOS)
$ dns.exe -host ifmo.ru    (Windows)

DNS Servers for ifmo.ru:
    ns.ifmo.ru.
    ns2.ifmo.ru.

Performing 5000 tests...
77.234.212.50    mail.ifmo.ru.
127.0.0.1        localhost.ifmo.ru.
77.234.221.75    ns2.ifmo.ru.
77.234.194.2     ns.ifmo.ru.
194.85.160.51    ns3.ifmo.ru.
194.85.160.54    ns4.ifmo.ru.
77.234.212.55    proxy.ifmo.ru.
77.234.212.18    my.ifmo.ru.
77.234.212.49    gw.ifmo.ru.
77.234.212.22    services.ifmo.ru.
77.234.214.163   cs.ifmo.ru.
77.234.212.68    lib.ifmo.ru.
...
```

# Valgrind
There are no memory management and threading bugs