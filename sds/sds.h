#ifndef __SDS_H
#define __SDS_H

#define SDS_MAX_PREALLOC (1024*1024)
#include  <sys/types.h>
#include <stdarg.h>

typedef char *sds;

typedef struct sdshdr{
    unsigned int len;
    unsigned int free;
    char buf[];
} sdshdr;

static inline size_t sdslen(const sds s){
    struct sdshdr *sh = (void*)(s-sizeof(struct sdshdr));
    return sh->len;
}

static inline size_t sdsavail(const sds s){
    struct sdshdr *sh = (void*)(s-sizeof(struct sdshdr));
    return sh->free;
}

//创建指定长度的sds
sds sdsnewlen(const void *init, size_t initlen);
//根据字符串创建sds
sds sdsnew(const char *init);
//设置sds为空串
sds sdsempty();
//内联函数获取sds的len
size_t sdslen(const sds s);
//获取sds的free
size_t sdsavail(const sds s);
//复制sds,根据sds的字符串重新创建一个
sds sdsdup(const sds s);
//free sds
void sdsfree(const sds s);
//更新sds为其实际的长度
void sdsupdatelen(sds s);
//清空sds
void sdsclear(sds s);
//增加sds的可用空间
sds sdsMakeRoomFor(sds s, size_t addlen);
//清理free space
sds sdsRemoveFreeSpace(sds s);
//返回sds所有分配的所有空间大小
size_t sdsAllocSize(sds s);
//在现有内存空间中进行操作,不涉及内存的分配
void sdsIncrLen(sds s, int incr);
//将sds的buf设置为空,且增加len的空间
sds sdsgrowzero(sds s, size_t len);
//在现有的buf上追加len长度的字符串t
sds sdscatlen(sds s, const void*t, size_t len);
//在现有的buf上追加另一个sds t
sds sdscat(sds s, const char* t);
//将字符串t的前len个字符拷贝到sds s中
sds sdscpylen(sds s, const void*t , size_t len);
//将整个字符串t都拷贝到sds s中
sds sdscpy(sds s, const void*t);
//将整数保存在缓存s中,结果返回整数的位数
int sdsll2str(char *s, long long value);
//将无符号整数保存在缓存s中,结果返回整数的位数
int sdsull2str(char *s, unsigned long long value);
//将整数转换为sds对象
sds sdsfromlonglong(long long value);
//由参数format格式化的字符串追加到sds s中
sds sdsvprintf(sds s, const char *fmt, va_list ap);
//将变量参数格式到sds中
sds sdscatprintf(sds s, const char *fmt, ...);
//不使用sprintf的格式字符串
sds sdscatfmt(sds s, const char*fmt,...);
//删除sds两头的所有在cset中的字符
sds sdstrim(sds s, const char *cset);
//按照范围获取start和end之间的字符串,支持负整数
void sdsrange(sds s, int start, int end);
//全部转换为小写,在config的使用,command的大小写在dict匹配的时候进行忽略,因此不需要转换
void sdstolower(sds s);
//全部转换为大写,stand by,暂时没有使用
void sdstoupper(sds s);
//sds比较
int sdscmp(const sds s1, const sds s2);
//根据step字符串进行sds的切换,返回sds数组
sds *sdssplitlen(const char*s, int len, const char*step, int steplen, int *count);
//sdssplitlen切分后内存的回收
void sdsfreesplites(sds *tokens, int count);
//将字符串p进行特殊字符格式并追加到s中
sds sdscatrepr(sds s, const char*p ,size_t len);
//将命令行参数处理成redis可识别的sds数组并返回argc参数数目
sds *sdssplitargs(const char *line, int *argc);
//将from字符串中对应的字符替换成to字符串对应的字符
sds sdsmapchars(sds s, const char*from, const char*to, size_t setlen);
//将参数按照step进行连接返回sds字符串
sds sdsjoin(char **args, int argc, char *step);

#endif
