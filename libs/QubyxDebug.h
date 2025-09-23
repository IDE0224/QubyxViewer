#ifndef QBXDEBUG_H
#define QBXDEBUG_H
//#pragma one

#include <QDebug>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <ctime>


#define QBXDBG_MESSAGE_SIGNATURE "@@"
#define QBXDBG_CONTINUME_MESSAGE_SIGNATURE "."
#define QBXDBG_INDENT "    "


#include "PrintVariable.h"
#include "PrintExpression.h"


template <class CharType>
void __print_return_header(const CharType *function)
{
    qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<"Return"<<function;
}

class __DebugOutput
{
public:
    template <class Type>
    __DebugOutput &operator ()(Type arg)
    {
        __print_variable<Type>("", "", arg);
        return *this;
    }
};


inline void __dump(void *memory, size_t size)
{
    QDebug debugOut(QtDebugMsg);


    if (size == size_t(-1))
    {
        debugOut<<"@@"<<"Dump: bad size";
        return;
    }


    const char *table="0123456789ABCDEF";
    unsigned char *currentByte;


    debugOut<<"@@ Dump: ";
    currentByte = (unsigned char*)memory;
    for (size_t itr = 0; itr<size; ++itr)
    {
        debugOut.nospace()<<table[*currentByte>>4]<<table[*currentByte&0xf]<<' ';
        ++currentByte;
    }
}


#define $ qDebug()<<QBXDBG_MESSAGE_SIGNATURE

#define $P(expression) \
            {qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<(expression);}

#define $L { qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<__FILE__<<","<<__LINE__; }

#define $TI(...) { qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<#__VA_ARGS__; __VA_ARGS__ }

#define $T(...) \
            {qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<#__VA_ARGS__;} __VA_ARGS__

#define $FI(...) \
            {qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<__FILE__<<","<<__LINE__<<":"<<#__VA_ARGS__; __VA_ARGS__}

#define $F(...) \
            {qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<__FILE__<<","<<__LINE__<<":"<<#__VA_ARGS__;} __VA_ARGS__

#define $C \
            {qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<"Call"<<__PRETTY_FUNCTION__;} __DebugOutput()

#define $R(...) \
            (__print_return_header(__PRETTY_FUNCTION__), __print_variable("", "", (__VA_ARGS__)))

#define $RX(...) \
            (__print_return_header(__PRETTY_FUNCTION__), __print_expression("", "", (__VA_ARGS__)))

#define $V(variable) \
            (__print_watch_variable_header(#variable), __print_variable("", "", (variable)))

#define $X(...) \
            (__print_watch_expression_header(#__VA_ARGS__), __print_expression("", "", (__VA_ARGS__)))

#define $D(address, size) \
            __dump(address, size)

#define $M(...) \
            {std::clock_t __start, __end; \
            __start = std::clock(); \
            {__VA_ARGS__} \
            __end = std::clock(); \
            qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<"Code at"<<__FILE__<<","<<__LINE__<<"executed"<<__end-__start<<"ms";}

#define $N(condition) \
            {if (condition) qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<"Error in"<<__FILE__<<", at"<<__LINE__;}

#define $E(condition) \
            {if (condition) qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<"Error in"<<__FILE__<<", at"<<__LINE__<<": "<<#condition;}

#define $B(condition) \
            {if (condition) qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<"Error in"<<__FILE__<<", at"<<__LINE__; asm("int $3");}

#define $EE() \
            {qDebug()<<QBXDBG_MESSAGE_SIGNATURE<<"You found Easter Egg!";}

#endif	/* QBXDEBUG_H */
