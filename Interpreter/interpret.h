
#ifndef INTERPRET_H_
#define INTERPRET_H_

#include "DataValue.h"
#include "util/string_view.h"

#include <gsl/span>

#include <deque>
#include <memory>
#include <vector>

#include <QString>
#include <QtAlgorithms>

class DocumentWidget;
struct DataValue;
struct Program;
struct Symbol;

// Maximum stack size
constexpr int STACK_SIZE = 1024;

// Special value for the send_event field of events passed to action routines.  Tells them that they were called from a macro
constexpr int MACRO_EVENT_MARKER = 2;

#define ARRAY_DIM_SEP "\034"

enum SymTypes {
    CONST_SYM,
    GLOBAL_SYM,
    LOCAL_SYM,
    ARG_SYM,
    PROC_VALUE_SYM,
    C_FUNCTION_SYM,
	MACRO_FUNCTION_SYM
};

#define N_OPS 43
enum Operations {
	OP_RETURN_NO_VAL,
	OP_RETURN,
	OP_PUSH_SYM,
	OP_DUP,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_NEGATE,
	OP_INCR,
	OP_DECR,
	OP_GT,
	OP_LT,
	OP_GE,
	OP_LE,
	OP_EQ,
	OP_NE,
	OP_BIT_AND,
	OP_BIT_OR,
	OP_AND,
	OP_OR,
	OP_NOT,
	OP_POWER,
	OP_CONCAT,
	OP_ASSIGN,
	OP_SUBR_CALL,
	OP_FETCH_RET_VAL,
	OP_BRANCH,
	OP_BRANCH_TRUE,
	OP_BRANCH_FALSE,
	OP_BRANCH_NEVER,
	OP_ARRAY_REF,
	OP_ARRAY_ASSIGN,
	OP_BEGIN_ARRAY_ITER,
	OP_ARRAY_ITER,
	OP_IN_ARRAY,
	OP_ARRAY_DELETE,
	OP_PUSH_ARRAY_SYM,
	OP_ARRAY_REF_ASSIGN_SETUP,
	OP_PUSH_ARG,
	OP_PUSH_ARG_COUNT,
	OP_PUSH_ARG_ARRAY
};

enum ExecReturnCodes {
	MACRO_TIME_LIMIT, 
	MACRO_PREEMPT, 
	MACRO_DONE, 
	MACRO_ERROR
};

union Inst {
	int (*func)();
	int value;
	Symbol *sym;
};

//------------------------------------------------------------------------------

/* symbol table entry */
struct Symbol {
	std::string name;
	SymTypes    type;
	DataValue   value;
};

struct Program {

    ~Program() {
        qDeleteAll(localSymList);
    }

    std::deque<Symbol *> localSymList;
    std::vector<Inst>    code;
};

/* Information needed to re-start a preempted macro */
struct MacroContext {
    DataValue Stack[STACK_SIZE];             // the stack
    DataValue *StackP             = nullptr; // next free spot on stack
    DataValue *FrameP             = nullptr; // frame pointer (start of local variables for the current subroutine invocation)
    Inst *PC                      = nullptr; // program counter during execution
    DocumentWidget *RunDocument   = nullptr; // document from which macro was run
    DocumentWidget *FocusDocument = nullptr; // document on which macro commands operate
};

void InitMacroGlobals();
void CleanupMacroGlobals();

bool ArrayInsert(DataValue *theArray, const std::string &keyStr, DataValue *theValue);
void ArrayDelete(DataValue *theArray, const std::string &keyStr);
void ArrayDeleteAll(DataValue *theArray);
int ArraySize(DataValue *theArray);
bool ArrayGet(DataValue *theArray, const std::string &keyStr, DataValue *theValue);
int ArrayCopy(DataValue *dstArray, DataValue *srcArray);

/* Routines for creating a program, (accumulated beginning with
   BeginCreatingProgram and returned via FinishCreatingProgram) */
bool AddBranchOffset(Inst *to, const char **msg);
bool AddBreakAddr(Inst *addr);
bool AddContinueAddr(Inst *addr);
bool AddImmediate(int value, const char **msg);
bool AddOp(int op, const char **msg);
bool AddSym(Symbol *sym, const char **msg);
Inst *GetPC();
Program *FinishCreatingProgram();
Symbol *InstallIteratorSymbol();
Symbol *InstallStringConstSymbol(view::string_view str);
Symbol *InstallSymbol(const std::string &name, SymTypes type, const DataValue &value);
Symbol *InstallSymbolEx(const QString &name, enum SymTypes type, const DataValue &value);
Symbol *LookupStringConstSymbol(view::string_view value);
Symbol *LookupSymbolEx(const QString &name);
Symbol *LookupSymbol(view::string_view name);
void BeginCreatingProgram();
void FillLoopAddrs(Inst *breakAddr, Inst *continueAddr);
void StartLoopAddrList();
void SwapCode(Inst *start, Inst *boundary, Inst *end);

/* Routines for executing programs */
int ExecuteMacroEx(DocumentWidget *document, Program *prog, gsl::span<DataValue> arguments, DataValue *result, std::shared_ptr<MacroContext> &continuation, QString *msg);
int ContinueMacroEx(std::shared_ptr<MacroContext> &continuation, DataValue *result, QString *msg);
void RunMacroAsSubrCall(Program *prog);
void PreemptMacro();

Symbol *PromoteToGlobal(Symbol *sym);
void ModifyReturnedValueEx(const std::shared_ptr<MacroContext> &context, const DataValue &dv);
DocumentWidget *MacroRunDocumentEx();
DocumentWidget *MacroFocusDocument();
void SetMacroFocusDocument(DocumentWidget *document);

/* function used for implicit conversion from string to number */
bool StringToNum(const std::string &string, int *number);
bool StringToNum(const QString &string, int *number);

#endif
