#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sds.h"

sds sdsnewlen(const void*init, size_t initlen){
    struct sdshdr *sh;

    //init为空,也生成对应的sdshdr对象
    if(init)
        sh = malloc(sizeof(*sh)+initlen+1);
    else
        sh = calloc(sizeof(*sh)+initlen+1, 1);

    if(sh == NULL) return NULL;

    sh->len = initlen;
    sh->free = 0;
    if(initlen&&init)
        memcpy(sh->buf, init, initlen);
    sh->buf[initlen]='\0';

    return (char*)sh->buf;
}

sds sdsempty(void){
    return sdsnewlen("",0);
}

sds sdsnew(const char * init){
    size_t initlen = (init == NULL) ? 0 : strlen(init);
    return sdsnewlen(init, initlen);
}

sds sdsdup(const sds s){
    return sdsnewlen(s, strlen(s));
}

void sdsfree(const sds s){
    if(s == NULL)return;

    free((s-sizeof(struct sdshdr)));
}

void sdsupdatelen(sds s){
    struct sdshdr * sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
    size_t reallen = strlen(s);

    sh->len = reallen;
    sh->free += reallen;
}

void sdsclear(sds s){
    struct sdshdr *sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
    size_t reallen = 0;
    sh->free += sh->len;
    sh->len = 0;
    sh->buf[0]='\0';
}

sds sdsMakeRoomFor(sds s, size_t addlen){
    struct sdshdr *sh;
    size_t len, newlen;
    size_t free = sdsavail(s);

    sh = (struct sdshdr*)(s-sizeof(struct sdshdr));

    len = sh->len;

    if(addlen < free) return s;
    
    newlen = len + addlen;
    //预分配
    if(newlen < SDS_MAX_PREALLOC)
        newlen *= 2;
    else
        newlen += SDS_MAX_PREALLOC;

    struct sdshdr *newsh;
    //扩展内存
    newsh = realloc(sh, newlen);

    if(newsh == NULL)return NULL;

    newsh->free = (newlen-len);

    return newsh->buf;
}

sds sdsRemoveFreeSpace(sds s){
    struct sdshdr *sh;
    size_t len;
    sh = (struct sdshdr*)(s - sizeof(struct sdshdr));

    len = sh->len;
    sh = realloc(sh, sizeof(struct sdshdr)+len+1);
    sh->free = 0;

    return sh->buf;
}

size_t sdsAllocSize(sds s){
    struct sdshdr *sh;
    sh = (struct sdshdr*)(s - sizeof(struct sdshdr));

    return sizeof(*sh)+sh->len+sh->free+1;
}

void sdsIncrLen(sds s, int incr){
    //添加sdsIncr的函数操作,其在现有内存空间中进行操作,不涉及内存的分配
    //故有别与sdsMakeRoomFor
    struct sdshdr *sh;
    sh = (struct sdshdr*)(s - sizeof(struct sdshdr));

    if(incr>=0){
        //增长的空间小于free,可以无需添加即可
        assert(sh->free >= (unsigned int)incr);
    }else{
        //现有的len如果大于incr的长度,才可以操作
        assert(sh->len >= (unsigned int)(-incr));
    }

    sh->len += incr;
    sh->free -= incr;

    s[sh->len] = '\0';
}

sds sdsgrowzero(sds s, size_t len){
    struct sdshdr *sh = (void*)(s-sizeof(struct sdshdr));
    size_t curlen = sh->len;
    size_t totallen;

    if(len <= curlen) return s;
    s = sdsMakeRoomFor(s, len-curlen);
    if(s == NULL) return NULL;

    sh = (void*)(s-sizeof(struct sdshdr));
    memset(sh->buf, 0 ,sh->len);

    totallen = sh->len + sh->free;
    sh->len = len;
    sh->free = totallen - sh->len;

    return s;
}

sds sdscatlen(sds s, const void *t, size_t len){
    struct sdshdr *sh;
    size_t currlen = sdslen(s);

    s = sdsMakeRoomFor(s,len);
    if(s == NULL) return NULL;

    sh = (void*)(s-sizeof(*sh));
    memcpy(s+currlen, t, len);
    sh->len = currlen + len;
    sh->free -= len;

    sh->buf[sh->len]='\0';

    return s;
}

sds sdscat(sds s, const sds t){
    return sdscatlen(s, t, strlen(t));
}

sds sdscpylen(sds s, const void *t, size_t len){
    struct sdshdr *sh = (void*)(s-sizeof(struct sdshdr));
    size_t totallen = sh->free+sh->len;

    if(totallen<len){
        s = sdsMakeRoomFor(s, len);
        sh = (void*)(s-sizeof(struct sdshdr));
        if(sh == NULL)
            return NULL;
        totallen = sh->free+sh->len;
    }

    memcpy(s, t, len);
    s[len]='\0';
    sh->len = len;
    sh->free = totallen - len;

    return s;
}

sds sdscpy(sds s , const void *t){
    return sdscpylen(s, t, strlen(t));
}

int sdsll2str(char *s, long long value){
    char *p, aux;
    unsigned long long v;
    size_t l;
    //按10取模将单个数字保存到buf中 
    v = (value < 0) ? -value : value;
    p = s;
    do{
        *p++ = '0' + v%10;
        v /= 10;
    }while(v);

    if(value<0)
        *p++ = '-';
    *p = '\0';

    l = p-s;

    //翻转字符串,需要排除结尾的\0
    p--;
    while(s<p){
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }

    return l;
}

int sdsull2str(char *s, unsigned long long value){
    char *p, aux;
    size_t l;
    p = s;
    //获取各位数字保存到s中
    do{
        *p++ = '0' + value%10;
        value /= 10;
    }while(value);

    l = p-s;
    *p = '\0';

    //翻转字符串
    p--;

    while(s<p){
        aux = *s;
        *s = *p;
        *p = aux;

        s++;
        p--;
    }

    return l;
}

#define SDS_LLSTR_SIZE 21
sds sdsfromlonglong(long long value){
    char buf[SDS_LLSTR_SIZE];
    int newlen;
    newlen = sdsll2str(buf, value);

    return sdsnewlen(buf, newlen);
}

sds sdsvprintf(sds s, const char *fmt, va_list ap){
   va_list cpy;
   char staticbuf[1024], *buf=staticbuf, *t;
   size_t buflen = strlen(fmt)*2;

   //预分配
   if(buflen >= sizeof(staticbuf)){
        buf = malloc(buflen);
        if(buf==NULL) return NULL;
   }else{
       buflen = sizeof(staticbuf);
   }

   while(1){
       //只是起到标记作用,如果超过设置1024,重新分配内存
        buf[buflen-3] = '\0';
        va_copy(cpy, ap);
        vsnprintf(buf, buflen, fmt, cpy);
        va_end(cpy);

        if(buf[buflen-3]!='\0'){
            if(buf != staticbuf) free(buf);
            buflen *= 2;
            buf = malloc(buflen);
            if(buf == NULL) return NULL;
            continue;
        }
        break;
   }

   t = sdscat(s, buf);
   if(buf != staticbuf) free(buf);

   return t;
}

sds sdscatprintf(sds s, const char *fmt, ...){
    va_list ap;
    char *t;

    va_start(ap, fmt);
    t = sdsvprintf(s, fmt, ap);
    va_end(ap);

    return t;
}

sds sdscatfmt(sds s, const char*fmt, ...){
   struct sdshdr*sh = (void *)(s - sizeof(struct sdshdr));
   size_t initlen = sdslen(s);

   va_list ap;
   const char *f = fmt;
   int i;

   va_start(ap, fmt);
   i = initlen;

   while(*f){
       char next, *str;
       unsigned int l;
       long long num;
       unsigned long long unum;

       if(sh->free == 0){
            s = sdsMakeRoomFor(s,1);
            sh = (void *)(s - sizeof(struct sdshdr));}

       switch(*f){
        case '%':
            next=*(f+1);
            f++;
            switch(next){
                case 's':
                case 'S':
                    str = va_arg(ap, char *);
                    l = (next == 's') ? strlen(str) : sdslen(str);
                    if(sh->free < l){
                        s = sdsMakeRoomFor(s, l);
                        sh = (void *)(s - sizeof(struct sdshdr));
                    }
                    memcpy(s+i, str, l);
                    sh->len += l;
                    sh->free -=l;
                    i += l;
                    break;
                case 'i':
                case 'I':
                    if(next == 'i')
                        num = va_arg(ap, int);
                    else
                        num = va_arg(ap, long long);
                    {
                        char buf[SDS_LLSTR_SIZE];
                        l = sdsll2str(buf, num);
                        if(sh->free < l){
                            s = sdsMakeRoomFor(s, l);
                            sh = (void *)(s - sizeof(struct sdshdr));
                        }

                        memcpy(s+i, buf, l);
                        sh->len += l;
                        sh->free -= l;
                        i += l;
                    }
                    break;
                case 'u':
                case 'U':
                    if (next == 'u')
                        unum = va_arg(ap, unsigned int);
                    else
                        unum = va_arg(ap, unsigned long long);
                    {
                        char buf[SDS_LLSTR_SIZE];
                        l = sdsull2str(buf, unum);
                        if(sh->free < l){
                            s = sdsMakeRoomFor(s, l);
                            sh = (void *)(s - sizeof(struct sdshdr));
                        }
                        memcpy(s+i, buf, l);
                        sh->len += l;
                        sh->free -= l;
                        i += l;
                    }
                    break;
                default:
                    s[i++] = next;
                    sh->len +=1;
                    sh->free -=1;
                    break;
            }
        break;
        default:
            s[i++] = *f;
            sh->len +=1;
            sh->free -=1;
            break;
       }
       f++;
   }
   va_end(ap);
   s[i++] = '\0';
   return s;
}

sds sdstrim(sds s, const char *cset){
   struct sdshdr *sh = (void *)(s-sizeof(struct sdshdr));
   char *start, *end;
   char *sp, *ep;
   size_t len;

   start = sp = s;
   end = ep = s + sdslen(s) - 1;

   while(sp<=end && strchr(cset, *sp)) sp++;
   while(ep>start && strchr(cset, *ep)) ep--;
   
   len = (sp>ep) ? 0 : ((ep-sp)+ 1);

   if(sh->buf != sp) memmove(sh->buf, sp, len);
   sh->buf[len]='\0';
   sh->len = len;
   sh->free = sh->free+(sh->len-len);

   return s;
}

void sdsrange(sds s, int start, int end){
    //start 从0开始,end从-1开始或者len-1开始
    //start和end均在[-len+1, len-1]之间
    struct sdshdr *sh = (void*)(s - sizeof(struct sdshdr));
    size_t newlen, len = sdslen(s);

    if(len==0)return;

    if(start<0){
        start = start + len;
        if(start < 0) start = 0;
    }

    if(end<0){
        end = end + len;
        if(end<0) end = 0;
    }

    newlen = (start > end) ? 0 : ((end-start)+1);
    //下面这段作用是防止start和end越界
    //例如:
    //sds r = sdsnew("Hello World")
    //sdsrange(r, 100, 200)
    //这种条件下,newlen=100,start<end
    //如果直接memmove话,会出现内存越界
    //再次检查start和end是否越界
    if(newlen != 0){
        if(start >= (signed)len)
            newlen = 0;
        else if(end >= (signed)len){
            end = len - 1;
            newlen = (start > end) ? 0 : ((end-start)+1);
        }
    }else{
        start = 0;
    }

    if(newlen)memmove(sh->buf, sh->buf+start, newlen);
    sh->buf[newlen]='\0';
    sh->len = newlen;
    sh->free = sh->free-newlen+len;
}

void sdstolower(sds s){
    size_t i = 0;
    size_t len = sdslen(s);

    for(i=0; i < len; i++){
        s[i] = tolower(s[i]);
    }
}

void sdstoupper(sds s){
    size_t i, len = sdslen(s);
    for(i = 0; i < len; i++){
        s[i] = toupper(s[i]);
    }
}

int sdscmp(const sds s1, const sds s2){
    size_t s1_len = sdslen(s1);
    size_t s2_len = sdslen(s2);
    size_t len = (s1_len < s2_len) ? s1_len : s2_len;

    int cmp;
    cmp = memcmp(s1,s2, len);

    if(cmp == 0)
        return s1_len - s2_len;
    return cmp;
}

sds* sdssplitlen(const char*s, int len, const char*step, int steplen, int *count){
    int elements=0, slots=5, start=0,j=0;
    sds * tokens;

    if(steplen<1 || len<=0) return NULL;

    tokens = malloc(sizeof(sds)*slots);
    if(tokens == NULL) return NULL;

    for(j = 0; j < (len-(steplen-1));j++){
        if(slots<elements+2){
            sds* newtokens;
            newtokens = realloc(tokens, sizeof(sds)*slots);
            if(newtokens == NULL) goto cleanup;
            slots *= 2;
            tokens = newtokens;
        }

        if((steplen==1&&*(s+j) == step[0]) || memcmp(s+j, step, steplen)==0){
            tokens[elements] = sdsnewlen(s+start,j-start);
            if(tokens[elements] == NULL) goto cleanup;
            start = j+steplen;
            elements++;
            j=j+steplen-1;
        }
    }

    tokens[elements] = sdsnewlen(s+start,len-start);
    elements++;
    *count = elements;
    return tokens;

cleanup:
    for(j=0; j < elements; j++)free(tokens[j]);
    free(tokens);
    *count=0;
    return NULL;
}
