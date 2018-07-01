#ifndef ANA_EXCEPTIONS_H
#define ANA_EXCEPTIONS_H

#define AnaTypeError     "TypeError"
#define AnaArgumentError "ArgumentError"
#define AnaRuntimeError  "RuntimeError"
#define AnaSystemError   "SystemError"
#define InvalidOperation "InvalidOperation"

extern char **ana__excep_location(void);
extern char **ana__except_type_location(void);

#define ana_excep (*ana__excep_location())
#define ana_except_type (*ana__except_type_location())

#define Ana_SetError(type, fmt, ...) do { \
  ana_except_type = (type); \
  ana_excep = ana_build_str(fmt,  ##__VA_ARGS__); \
} while(0)

#endif