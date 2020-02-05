#ifndef FILE_ULISP_H
#define FILE_ULISP_H

#include <Arduino.h>
#include <setjmp.h>
//#include <limits.h>


#include <EEPROM.h>
#if defined (ESP8266)
  #include <ESP8266WiFi.h>
#elif defined (ESP32)
  #include <WiFi.h>
#endif

#include <SPI.h>
#include <Wire.h>



// C Macros

#define nil                NULL
#define car(x)             (((object *) (x))->car)
#define cdr(x)             (((object *) (x))->cdr)

#define first(x)           (((object *) (x))->car)
#define second(x)          (car(cdr(x)))
#define cddr(x)            (cdr(cdr(x)))
#define third(x)           (car(cdr(cdr(x))))

#define push(x, y)         ((y) = cons((x),(y)))
#define pop(y)             ((y) = cdr(y))

#define integerp(x)        ((x) != NULL && (x)->type == NUMBER)
#define floatp(x)          ((x) != NULL && (x)->type == FLOAT)
#define symbolp(x)         ((x) != NULL && (x)->type == SYMBOL)
#define stringp(x)         ((x) != NULL && (x)->type == STRING)
#define characterp(x)      ((x) != NULL && (x)->type == CHARACTER)
#define streamp(x)         ((x) != NULL && (x)->type == STREAM)

#define mark(x)            (car(x) = (object *)(((uintptr_t)(car(x))) | MARKBIT))
#define unmark(x)          (car(x) = (object *)(((uintptr_t)(car(x))) & ~MARKBIT))
#define marked(x)          ((((uintptr_t)(car(x))) & MARKBIT) != 0)
#define MARKBIT            1

#define setflag(x)         (Flags = Flags | 1<<(x))
#define clrflag(x)         (Flags = Flags & ~(1<<(x)))
#define tstflag(x)         (Flags & 1<<(x))

// Constants

const int TRACEMAX = 3; // Number of traced functions
enum type { ZERO=0, SYMBOL=2, NUMBER=4, STREAM=6, CHARACTER=8, FLOAT=10, STRING=12, PAIR=14 };  // STRING and PAIR must be last
enum token { UNUSED, BRA, KET, QUO, DOT };
enum stream { SERIALSTREAM, I2CSTREAM, SPISTREAM, SDSTREAM, WIFISTREAM };

enum function { NIL, TEE, NOTHING, OPTIONAL, AMPREST, LAMBDA, LET, LETSTAR, CLOSURE, SPECIAL_FORMS, QUOTE,
DEFUN, DEFVAR, SETQ, LOOP, RETURN, PUSH, POP, INCF, DECF, SETF, DOLIST, DOTIMES, TRACE, UNTRACE,
FORMILLIS, WITHSERIAL, WITHI2C, WITHSPI, WITHSDCARD, WITHCLIENT, TAIL_FORMS, PROGN, IF, COND, WHEN,
UNLESS, CASE, AND, OR, FUNCTIONS, NOT, NULLFN, CONS, ATOM, LISTP, CONSP, SYMBOLP, STREAMP, EQ, CAR, FIRST,
CDR, REST, CAAR, CADR, SECOND, CDAR, CDDR, CAAAR, CAADR, CADAR, CADDR, THIRD, CDAAR, CDADR, CDDAR, CDDDR,
LENGTH, LIST, REVERSE, NTH, ASSOC, MEMBER, APPLY, FUNCALL, APPEND, MAPC, MAPCAR, MAPCAN, ADD, SUBTRACT,
MULTIPLY, DIVIDE, MOD, ONEPLUS, ONEMINUS, ABS, RANDOM, MAXFN, MINFN, NOTEQ, NUMEQ, LESS, LESSEQ, GREATER,
GREATEREQ, PLUSP, MINUSP, ZEROP, ODDP, EVENP, INTEGERP, NUMBERP, FLOATFN, FLOATP, SIN, COS, TAN, ASIN,
ACOS, ATAN, SINH, COSH, TANH, EXP, SQRT, LOG, EXPT, CEILING, FLOOR, TRUNCATE, ROUND, CHAR, CHARCODE,
CODECHAR, CHARACTERP, STRINGP, STRINGEQ, STRINGLESS, STRINGGREATER, SORT, STRINGFN, CONCATENATE, SUBSEQ,
READFROMSTRING, PRINCTOSTRING, PRIN1TOSTRING, LOGAND, LOGIOR, LOGXOR, LOGNOT, ASH, LOGBITP, EVAL, GLOBALS,
LOCALS, MAKUNBOUND, BREAK, READ, PRIN1, PRINT, PRINC, TERPRI, READBYTE, READLINE, WRITEBYTE, WRITESTRING,
WRITELINE, RESTARTI2C, GC, ROOM, SAVEIMAGE, LOADIMAGE, CLS, PINMODE, DIGITALREAD, DIGITALWRITE,
ANALOGREAD, ANALOGWRITE, DELAY, MILLIS, SLEEP, NOTE, EDIT, PPRINT, PPRINTALL, REQUIRE, LISTLIBRARY,
AVAILABLE, WIFISERVER, WIFISOFTAP, CONNECTED, WIFILOCALIP, WIFICONNECT, ENDFUNCTIONS };

// Typedefs

typedef unsigned int symbol_t;

typedef struct sobject {
  union {
    struct {
      sobject *car;
      sobject *cdr;
    };
    struct {
      unsigned int type;
      union {
        symbol_t name;
        int integer;
        float single_float;
      };
    };
  };
} object;

typedef object *(*fn_ptr_type)(object *, object *);

typedef struct {
  const char *string;
  fn_ptr_type fptr;
  uint8_t min;
  uint8_t max;
} tbl_entry_t;

typedef int (*gfun_t)();
typedef void (*pfun_t)(char);
typedef int PinMode;


#if defined(sdcardsupport)
  #include <SD.h>
  #define SDSIZE 172
#else
  #define SDSIZE 0
#endif


// Workspace
#define WORDALIGNED __attribute__((aligned (4)))
#define BUFFERSIZE 34  // Number of bits+2

#if defined(ESP8266)
  #define PSTR(s) s
  #define PROGMEM
  #define WORKSPACESIZE (3072-SDSIZE)     /* Cells (8*bytes) */
  #define EEPROMSIZE 4096                 /* Bytes available for EEPROM */
  #define SYMBOLTABLESIZE 512             /* Bytes */
  #define SDCARD_SS_PIN 10
  //uint8_t _end;
  typedef int BitOrder;

#elif defined(ESP32)
  #define WORKSPACESIZE (8000-SDSIZE)     /* Cells (8*bytes) */
  #define EEPROMSIZE 4096                 /* Bytes available for EEPROM */
  #define SYMBOLTABLESIZE 1024            /* Bytes */
  #define analogWrite(x,y) dacWrite((x),(y))
  #define SDCARD_SS_PIN 13
  //uint8_t _end;
  typedef int BitOrder;

#endif



extern object *tee;
extern object Workspace[];
extern char SymbolTable[];
extern char *SymbolTop;

extern object *GlobalEnv;
extern object *GCStack;


extern const tbl_entry_t lookup_table[];

extern const char notanumber[];
extern const char notastring[];
extern const char notalist[];
extern const char notproper[];
extern const char noargument[];
extern const char nostream[];
extern const char overflow[];
extern const char invalidpin[];
extern const char resultproper[];


// utils

extern jmp_buf exception;

void error (symbol_t fname, PGM_P string, object *symbol);
void error2 (symbol_t fname, PGM_P string);
void checkargs (symbol_t name, object *args);
int issymbol (object *obj, symbol_t n);
char *symbolname (symbol_t x);
void indent (int spaces, pfun_t pfun);
int isstream (object *obj);
object *readstring (char delim, gfun_t gfun);
int digitvalue (char d);
int pack40 (char *buffer);
boolean valid40 (char *buffer);
float checkintfloat (symbol_t name, object *obj);
boolean improperp (object *x);
int checkinteger (symbol_t name, object *obj);
char *cstringbuf (object *arg);
int listlength (symbol_t name, object *list);
int stringlength (object *form);
object *assoc (object *key, object *list);
object *delassoc (object *key, object **alist);
char nthchar (object *string, int n);
int checkchar (symbol_t name, object *obj);
void buildstring (char ch, int *chars, object **head);
char *cstring (object *form, char *buffer, int buflen);
object *lispstring (char *s);


// interpreter

extern unsigned int TraceFn[];
extern unsigned int TraceDepth[];
extern unsigned int Freespace;
extern uint8_t End;

uint8_t lookupmin (symbol_t name);
uint8_t lookupmax (symbol_t name);
char *lookupbuiltin (symbol_t name);
char *lookupsymbol (symbol_t name);
object *symbol (symbol_t name);
object *myalloc ();
void testescape ();
object *number (int n);
object *makefloat (float f);
object *character (char c);
int builtin (char* n);
object *newsymbol (symbol_t name);
int longsymbol (char *buffer);
object *value (symbol_t n, object *env);
object *findvalue (object *var, object *env);
void trace (symbol_t name);
void untrace (symbol_t name);
object *stream (unsigned char streamtype, unsigned char address);
object *apply (symbol_t name, object *function, object *args, object *env);
void repl (object *env);
void gc (object *form, object *env);
int compactimage (object **arg);
void initworkspace ();
boolean tracing (symbol_t name);
object *closure (int tc, symbol_t name, object *state, object *function, object *args, object **env);


// fns

enum flag { PRINTREADABLY, RETURNFLAG, ESCAPE, EXITEDITOR, LIBRARYLOADED, NOESC };
extern volatile char Flags;

object *tf_progn (object *form, object *env);
void initsleep ();


// io

extern const char LispLibrary[];
extern object *GlobalString;
extern int GlobalStringIndex;
extern unsigned int I2CCount;
extern char LastChar;
extern WiFiClient client;
extern WiFiServer server;
extern char BreakLevel;

void pserial (char c);
void pint (int i, pfun_t pfun);
void printobject (object *form, pfun_t pfun);
void pfstring (const char *s, pfun_t pfun);
void pstring (char *s, pfun_t pfun);
void pfl (pfun_t pfun);
void serialbegin (int address, int baud);
void serialend (int address);
void I2Cinit (bool enablePullup);
bool I2Cstart (uint8_t address, uint8_t read);
void I2Cstop (uint8_t read);
bool I2Crestart (uint8_t address, uint8_t read);
object *read (gfun_t gfun);
int glibrary ();
int gserial ();
gfun_t gstreamfun (object *args);
pfun_t pstreamfun (object *args);
void printstring (object *form, pfun_t pfun);
unsigned int saveimage (object *arg);
unsigned int loadimage (object *arg);
void autorunimage ();
void superprint (object *form, int lm, pfun_t pfun);
object *fn_pprint (object *args, object *env);
object *fn_pprintall (object *args, object *env);
int subwidthlist (object *form, int w);
void supersub (object *form, int lm, int super, pfun_t pfun);

inline void pln (pfun_t pfun) {
  pfun('\n');
}


// internal interpreter functions

object *cons (object *arg1, object *arg2);
boolean atom (object *x);
boolean listp (object *x);
boolean consp (object *x);
int eq (object *arg1, object *arg2);
object *eval (object *form, object *env);


#endif // FILE_ULISP_H
