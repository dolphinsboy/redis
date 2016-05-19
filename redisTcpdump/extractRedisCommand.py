#!/usr/bin/env python26

#########################################################################
#   Author by:      guosong@chuchujie.com                               #
#   Description:    extract Redis Commands from tcpdump file            #
#   Created:        2016-05-19 21:09:00                                 #
#########################################################################

import os
import sys
import re
import subprocess
from optparse import OptionParser
from functools import wraps
import errno
import signal
import time
import json

TCPDUMP_TIMEOUT=10

class TimeoutError(Exception):
    def __init__(self, message=None, **kwargs):
        self.message = message
        super(TimeoutError, self).__init__(**kwargs)

    def __str__(self):
        return self.message

def timeout(seconds=10, error_message=os.strerror(errno.ETIME)):
    def decorator(func):
        def _handle_timeout(signum, frame):
            raise TimeoutError(error_message)

        def wrapper(*args, **kwargs):
            signal.signal(signal.SIGALRM, _handle_timeout)
            signal.alarm(seconds)
            try:
                result = func(*args, **kwargs)
            finally:
                signal.alarm(0)
            return result

        return wraps(func)(wrapper)

    return decorator

def parse(line):
    time_ip_result = line.split(' ')
    Time = time_ip_result[0]
    ClientIP = time_ip_result[2]

    result = time_ip_result[-1].lower()

    redis_commands = ['get', 'set', 'setnx',
                      'setex', 'psetex', 'append', 'strlen',
                      'del', 'exists', 'setbit', 'getbit',
                      'setrange', 'getrange', 'substr', 'incr',
                      'decr', 'mget', 'rpush', 'lpush', 'rpushx',
                      'lpushx', 'linsert', 'rpop', 'lpop', 'brpop',
                      'brpoplpush', 'blpop', 'llen', 'lindex', 'lset', 'lrange', 'ltrim',
                      'lrem', 'rpoplpush', 'sadd', 'srem', 'smove',
                      'sismember', 'scard', 'spop', 'srandmember',
                      'sinter', 'sinterstore', 'sunion', 'sunionstore',
                      'sdiff', 'sdiffstore', 'smembers', 'sscan', 'zadd',
                      'zincrby', 'zrem', 'zremrangebyscore', 'zremrangebyrank',
                      'zremrangebylex', 'zunionstore', 'zinterstore',
                      'zrange', 'zrangebyscore', 'zrevrangebyscore',
                      'zrangebylex', 'zrevrangebylex', 'zcount',
                      'zlexcount', 'zrevrange', 'zcard', 'zscore',
                      'zrank', 'zrevrank', 'zscan', 'hset', 'hsetnx',
                      'hget', 'hmset', 'hmget', 'hincrby', 'hincrbyfloat',
                      'hdel', 'hlen', 'hkeys', 'hvals', 'hgetall',
                      'hexists', 'hscan', 'incrby', 'decrby', 'incrbyfloat', 'getset', 'mset',
            'msetnx', 'randomkey', 'select', 'move', 'rename', 'renamenx', 'expire', 'expireat', 'pexpire', 'pexpireat', 'keys', 'scan', 'dbsize', 'auth', 'ping', 'echo', 'save', 'bgsave', 'bgrewriteaof', 'shutdown', 'lastsave', 'type', 'multi', 'exec', 'discard', 'sync', 'psync', 'replconf', 'flushdb', 'flushall', 'sort', 'info', 'monitor', 'ttl', 'pttl', 'persist', 'slaveof', 'role', 'debug', 'config', 'subscribe', 'unsubscribe', 'psubscribe', 'punsubscribe', 'publish', 'pubsub', 'watch', 'unwatch', 'cluster', 'restore', 'restore-asking', 'migrate', 'asking', 'readonly', 'readwrite', 'dump', 'object', 'client', 'eval', 'evalsha', 'slowlog', 'script', 'time', 'bitop', 'bitcount', 'bitpos', 'wait', 'command', 'pfselftest', 'pfadd', 'pfcount', 'pfmerge', 'pfdebug', 'latency']

    command = "unkown"
    for cmd in redis_commands:
        if re.search(cmd, result):
            command = cmd
            print "Time: %s ClientIP: %s Command:%s" % (Time, ClientIP, command)
            break

def extractCommand(filename):
    file_object = open(filename)
    content_total = ""
    command_list = []

    command_line = []
    not_first  = False
    count = 0

    for line in file_object:
        line = line.strip()
        if re.match(r'^0x', line):
            command_line.append(line.split()[-1])
        else:
            if not_first:
                #get whole line
                parse(''.join(command_line))
                command_line = []
                command_line.append(line)
                count = count + 1
            else:
                command_line.append(line)
                not_first = True

@timeout(seconds=TCPDUMP_TIMEOUT,error_message='tcpdump call timeout')
def tcpdump(eth,port,count):
    OUTPUT_FILE="./tcpdump_mcq_%s.txt" %(port)
    if os.path.exists(OUTPUT_FILE):
        OUTPUT_FILE_BAK = "./tcpdump_mcq_%s.txt.bak" %(port)
        os.rename(OUTPUT_FILE, OUTPUT_FILE_BAK)
    system_cmd = """ sudo tcpdump -i %s dst port %d -nn -s 0 -A -X -c %d > %s  """ %(eth,port,count,OUTPUT_FILE)
    pipe = subprocess.Popen(system_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True,stdin=subprocess.PIPE)
    stdout, stderr = pipe.communicate()
    return OUTPUT_FILE


def usage():
    print "Usage 1: extractCommand.py <-i eth*> <-p 86**> <-c count> |grep HGET|HGETALL ... * Please max(count) <100w, Or up to you !";
    print "Usage 2: extractCommand.py <-f filename(tcpdump)> |grep HGET|HGETALL|HSET ....";
    sys.exit(1)

if __name__ == "__main__" :
    parser = OptionParser()
    parser.add_option("-p","--port",type="int",dest="port")
    parser.add_option("-c","--count",type="int",dest="count")
    parser.add_option("-i","--eth",type="string",dest="eth")
    parser.add_option("-f","--file",type="string",dest="filename")
    parser.add_option("-t","--timeout",type="string",dest="timeout")

    (options, agrgs) = parser.parse_args()

    result = {}
    try:
        if options.filename:
            filename = options.filename
            extractCommand(filename)
        else:
            if not options.port or not options.eth:
                usage()
            else:
                port = options.port
                eth = options.eth
                count = 1000 if not options.count else options.count
                TCPDUMP_TIMEOUT = 10 if not options.timeout else options.timeout
                filename = tcpdump(eth,port,count)
                command_list = extractCommand(filename)

                result['status']= 0
                result['command_list'] = command_list
    except Exception,e:
        result['status'] = -1
        result['msg'] = str(e)
    finally:
        #print json.dumps(result)
        pass
