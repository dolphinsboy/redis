###  1. Redis-3.0.5 源码目录结构

redis基本上所有代码都放在src中，如下按照一定规则进行分类，便于日后有针对性查看。

```bash
├── basicinfo
├── cluster
├── compatible
├── data
├── deps
├── event
├── io
├── main
├── memory
├── net
├── others
├── sentinel
├── struct
├── test
├── tool
└── wrapper
```

### 2. basicinfo 基础信息

| 文件名        | 说明           |
| ------------- |:-------------| 
| asciilogo.h     | redis log字符串 | 
| version.h     | redis 版本信息      | 

### 3. cluster RedisCluster相关

| 文件名        | 说明           |
| ------------- |:-------------| 
| cluster.c     | redis  cluster 具体实现| 
| cluster.h     | redis cluster头文件      | 

### 4. compatible 兼容性相关

| 文件名        | 说明           |
| ------------- |:-------------| 
| fmacros.h     |兼容mac问题| 
| solarisfixes.h     |     兼容solaris | 

### 5. data 数据操作相关

| 文件名        | 说明           |
| ------------- |:-------------| 
| aof.c     |AOF日志相关| 
|config.c     |     解析配置文件并将其存储在redis server中 |
| db.c     |对Redis内存数据库相关的操作| 
| multi.c     |Redis事务相关操作| 
| rdb.c     |Redis RDB相关| 
| replication.c     |Redis主从同步相关|
| slowlog.c |Redis慢查询相关|
 
 
### 6. deps 其他一些辅助依赖包
 
| 文件名        | 说明           |
| ------------- |:-------------| 
| hiredis     |Redis Client 相关 |
| jemalloc  |   jemalloc内存相关 | 
| linenoise  |  [支持行的缓存、Tab控制等](https://github.com/antirez/linenoise) |
| lua  |   Lua脚本操作相关|

### 7. event IO多路复用事件操作相关

| 文件名        | 说明           |
| ------------- |:-------------| 
| ae.c     |各种事件的封装，支持Select、Epoll、Kqueue、Evport |
| ae_epoll.c     |Epoll事件的封装|
| ae_select.c     |Select事件的封装|
| ae_kqueue.c    |Kqueue事件的封装|
| ae_evport.c    | Evport事件的封装|

### 8.IO 相关

| 文件名        | 说明           |
| ------------- |:-------------| 
| bio.c     |后续线程异步刷新aof相关io |
| rio.c  |   Buffer IO相关 | 

### 9 . main Redis Bin目录下相关程序

| 文件名        | 说明           |
| ------------- |:-------------| 
| redis.c     |主函数入口，redis server相关 |
| redis-cli.c     |redis client |
| redis-benchmark.c     |redis benchmark  |
| redis-check-aof.c     |AOF完整性的检查 |
| redis-check-dump.c    |RDB文件完整性检查 |
| redis-trib.rb    |Redis集群管理操作脚本，Ruby写的 |

### 10. Memory内存操作相关

| 文件名        | 说明           |
| ------------- |:-------------| 
|zmalloc.c   |封装兼容libc、tcmalloc、jemalloc |

### 11. net 网络操作相关
| 文件名        | 说明           |
| ------------- |:-------------| 
|anet.c   |封装网络基本操作相关，例如connect、read、write等 |
|networking.c   |Redis所有和网络相关的操作，例如初始化redis client等 |


### 12.sentinel HA哨兵
| 文件名        | 说明           |
| ------------- |:-------------| 
|sentinel.c   |哨兵相关 |

### 13. struct 底层数据结构以及Redis5个数据结构

| 文件名        | 说明           |
| ------------- |:-------------| 
|sds.c   |字符串封装的sds结构|
|adlist.c   |双向链表结构|
|dict.c   |使用hash[2]实现字典结构，是hashset、set以及sorted_set底层存储|
|ziplist.c   |压缩list|
|zipmap.c   |压缩map|
|t_string.c   |string的相关操作|
|t_list.c   |list的相关操作|
|t_hash.c   |hash的相关操作|
|t_set.c   |set的相关操作|
|t_zset.c   |sorted set以及skiplist的相关操作|

### 14. test 测试类
| 文件名        | 说明           |
| ------------- |:-------------| 
|memtest.c   |内存检测的工具|
|testhelp.h   |C的min型测试框架,在sds中使用|

### 16. tool 基础工具类
| 文件名        | 说明           |
| ------------- |:-------------| 
|bitops.c   |底层的bit相关操作函数|
|blocked.c   |redis client相关block以及unblock操作|
|crc16.c   |crc16的实现，主要用于redis cluster分片使用|
|crc64.c   |crc64的实现，主要用于rdb文件校验值的生成|
|debug.c   |用于调试时使用|
|endianconv.c   |不同系统大小端转换|
|lzf_c.c   |压缩算法系列-压缩|
|lzf_d.c   |压缩算法系列-解压缩|
|rand.c   |随机数相关操作|
|release.c   |用于发布时使用，git相关|
|setproctitle.c   |根据使用mode不同，设置不同的进程命令，例如cluster、sentinel等|
|sha1.c   |sha加密算法的实现|
|util.c   |基础操作库，例如字符串匹配、1Gb转换为对应的数据、字符串数字转换等|

### 17. wrapper 封装类
| 文件名        | 说明           |
| ------------- |:-------------| 
|hyperloglog.c   |HyperLogLog 结构，用来作基数统计|
|intset.c   |整数范围内的使用set，并包含相关set操作|
|latency.c   |延迟类相关操作|
|notify.c   |通知类相关操作|
|object.c   |用于创建和释放redisObject对象|
|pqsort.c   |快排的实现|
|pubsub.c   |订阅和发布相关的实现|
|scripting.c   |Lua操作脚本相关|
|sort.c   |排序相关，与pqsort使用场景不同|
|syncio.c   |用于同步Socket和文件I/O操作|

### 18. others 其他各种
| 文件名        | 说明           |
| ------------- |:-------------| 
|mkreleasehdr.sh   |用于发布使用|
|valgrind.sup   |作用未知|


