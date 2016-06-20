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
    {
        int i;
        for(i=0; i < elements; i++)sdsfree(tokens[i]);
        free(tokens);
        *count=0;
        return NULL;
    }
}

void sdsfreesplites(sds *tokens, int count){
    if(tokens == NULL)
        return;

    while(count--)
        sdsfree(tokens[count]);

    free(tokens);
}

sds sdscatrepr(sds s, const char *p, size_t len){
    s = sdscatlen(s, "\"", 1);
    while(len--){
        switch(*p){
            case '\\':
            case '"':
                s = sdscatprintf(s,"\\%c", *p);
                break;
            case '\n': s = sdscatlen(s,"\\n",2);break;
            case '\r': s = sdscatlen(s,"\\r",2);break;
            case '\b': s = sdscatlen(s,"\\b",2);break;
            case '\a': s = sdscatlen(s,"\\a",2);break;
            case '\t': s = sdscatlen(s,"\\t",2);break;
            default:
                if(isprint(*p))
                    s = sdscatprintf(s, "%c", *p);
                else
                    s = sdscatprintf(s,"\\x%02x", (unsigned char)*p);
                break;
        }

        p++;
    }

    s = sdscatlen(s, "\"", 1);

    return s;
}

//以下两个函数直接从源码中拷贝过来的
/* Helper function for sdssplitargs() that returns non zero if 'c'
 * is a valid hex digit. */
int is_hex_digit(char c) { 
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F');
}

/* Helper function for sdssplitargs() that converts a hex digit into an
 * integer from 0 to 15 */
int hex_digit_to_int(char c) { 
    switch(c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': case 'A': return 10;
    case 'b': case 'B': return 11;
    case 'c': case 'C': return 12;
    case 'd': case 'D': return 13;
    case 'e': case 'E': return 14;
    case 'f': case 'F': return 15;
    default: return 0;
    }    
}

//以下函数直接从源码拷贝过来
sds *sdssplitargs(const char *line, int *argc) {
    const char *p = line;
    char *current = NULL;
    char **vector = NULL;

    *argc = 0;
    while(1) {
        /* skip blanks */
        while(*p && isspace(*p)) p++;
        if (*p) {
            /* get a token */
            int inq=0;  /* set to 1 if we are in "quotes" */
            int insq=0; /* set to 1 if we are in 'single quotes' */
            int done=0;

            if (current == NULL) current = sdsempty();
            while(!done) {
                if (inq) {
                    //支持16进制的输入
                    if (*p == '\\' && *(p+1) == 'x' &&
                                             is_hex_digit(*(p+2)) &&
                                             is_hex_digit(*(p+3)))
                    {
                        unsigned char byte;

                        byte = (hex_digit_to_int(*(p+2))*16)+
                                hex_digit_to_int(*(p+3));
                        current = sdscatlen(current,(char*)&byte,1);
                        p += 3;
                    } else if (*p == '\\' && *(p+1)) {
                        char c;

                        p++;
                        switch(*p) {
                        case 'n': c = '\n'; break;
                        case 'r': c = '\r'; break;
                        case 't': c = '\t'; break;
                        case 'b': c = '\b'; break;
                        case 'a': c = '\a'; break;
                        default: c = *p; break;
                        }
                        current = sdscatlen(current,&c,1);
                    } else if (*p == '"') {
                        /* closing quote must be followed by a space or
                         * nothing at all. */
                        if (*(p+1) && !isspace(*(p+1))) goto err;
                        done=1;
                    } else if (!*p) {
                        /* unterminated quotes */
                        goto err;
                    } else {
                        current = sdscatlen(current,p,1);
                    }
                } else if (insq) {
                    if (*p == '\\' && *(p+1) == '\'') {
                        p++;
                        current = sdscatlen(current,"'",1);
                    } else if (*p == '\'') {
                        /* closing quote must be followed by a space or
                         * nothing at all. */
                        if (*(p+1) && !isspace(*(p+1))) goto err;
                        done=1;
                    } else if (!*p) {
                        /* unterminated quotes */
                        goto err;
                    } else {
                        current = sdscatlen(current,p,1);
                    }
                } else {
                    switch(*p) {
                    case ' ':
                    case '\n':
                    case '\r':
                    case '\t':
                    case '\0':
                        done=1;
                        break;
                    case '"':
                        inq=1;
                        break;
                    case '\'':
                        insq=1;
                        break;
                    default:
                        current = sdscatlen(current,p,1);
                        break;
                    }
                }
                if (*p) p++;
            }
            /* add the token to the vector */
            vector = realloc(vector,((*argc)+1)*sizeof(char*));
            vector[*argc] = current;
            (*argc)++;
            current = NULL;
        } else {
            /* Even on empty input string return something not NULL. */
            if (vector == NULL) vector = malloc(sizeof(void*));
            return vector;
        }
    }

err:
    while((*argc)--)
        sdsfree(vector[*argc]);
    free(vector);
    if (current) sdsfree(current);
    *argc = 0;
    return NULL;
}
//from和to的字符串长度都需要是一样的,setlen可以小于等于from的字符串长度
sds sdsmapchars(sds s, const char*from, const char*to, size_t setlen){
    int i,j,l = sdslen(s);

    for(j = 0; j < l; j++)
        for(i = 0; i < setlen; i++){
            if(s[j] == from[i])
                s[j] = to[i];
            
        }

    return s;
}
