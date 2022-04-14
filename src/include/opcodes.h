#ifndef COMO_VM_OPCODES
#define COMO_VM_OPCODES


#define IUNARYMINUS 0x00
#define ITHROW 0x01
#define JMP 0x02
#define JMPZ 0x03
#define TRY 0x04
#define SETUP_CATCH 0x05
#define LOAD_SUBSCRIPT 0x06
#define STORE_SUBSCRIPT 0x07
#define INITARRAY 0x08
#define INITOBJ 0x09
#define SETPROP 0x0A
#define GETPROP 0x0B
#define IRETURN 0x0C
#define LOAD_CONST 0x0D
#define STORE_NAME 0x0E
#define LOAD_NAME 0x0F
#define IDIV 0x10
#define IADD 0x11
#define ILTE 0x12
#define ITIMES 0x13
#define IMINUS 0x14
#define CALL 0x15
#define IUNARYPLUS 0x16
#define IUNARYNOT 0x17
#define IEQUAL 0x18
#define INEQUAL 0x19
#define IREM 0x1A
#define IIMPORT 0X1B
#define IIMPORTAS 0x1C
#define IIN       0x1D
#define BEGIN_LOOP 0x1E
#define END_LOOP   0x1F
#define EXIT_LOOP_CONTINUE 0x20
#define DEFINE_FUNCTION   0x21
#define DEFINE_CLASS      0x22
#define ILSHFT            0x23
#define IRSHFT            0x24
#define IGTE              0x25
#define IGT               0x26
#define ILT               0x27
#define CALL_METHOD       0x28
#define ILAND             0x29
#define ILOR              0x30
#define ITER              0x31
#define ITER_MV           0x32
#define JMPF              0x33
#define JMPT              0x34
#define ANA_LAST_OPCODE   0x35

#endif