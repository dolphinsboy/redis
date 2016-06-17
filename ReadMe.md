## 1. Perface

Learning Redis source code step by step

## 2. Directory 

+ Include Redis basic structure test
+ Include modified Redis source code 

```bash
.
├── adlist
├── ae
├── byteorder
├── dict
├── redis-3.0.5
├── redisTcpdump
└── sds
```

## 3. New Future

+ Forbidden dangerous command , if only connect from admin hosts

```bash
[guosong@dev-00 ~]$ redis-cli -h 10.30.6.49 -p 3050 
10.30.6.49:3050> flushall
OK
[ec2-user@service-test3 ~]$ redis-cli -h 10.30.6.49 -p 3050 
10.30.6.49:3050> flushall
(error) ERR 10.30.6.183 Forbidden admin hosts 11 listlen=3 cmp=-1
```

Only flushall command , other command easy to code.

+ config get admin_hosts

```bash
10.30.6.49:3050> config get admin_hosts
1) "admin_hosts"
2) "127.0.0.1 10.30.6.49 10.30.6.217"
```

+ config set admin_hosts

```bash
127.0.0.1:3050> config set admin-hosts "127.0.0.1 10.30.6.49"
OK
127.0.0.1:3050> config set admin-hosts "127.0.0.1 10.30.6.49 10.30.6.217"
OK
127.0.0.1:3050> config get admin-hosts
1) "admin-hosts"
2) "127.0.0.1 10.30.6.49 10.30.6.217"
127.0.0.1:3050> config set admin-hosts "127.0.0.1 10.30.6.49 10.30.6.217 127.0.0.1 10.30.6.49 10.30.6.217"
OK
127.0.0.1:3050> config get admin-hosts
1) "admin-hosts"
2) "127.0.0.1 10.30.6.49 10.30.6.217"
```

+ admin-hosts的参数输入方式

```bash
[guosong@dev-00 redis3050]$ grep 'admin-hosts' conf/r3050.conf  --color
admin-hosts "127.0.0.1" "10.30.6.49" "10.30.6.217"
[guosong@dev-00 redis3050]$ echo "config get *" | redis-cli -p 3050  |grep -A 1 'admin-hosts' --color
admin-hosts
127.0.0.1 10.30.6.49 10.30.6.217
```

关键点在于sds中的sdssplitargs函数
