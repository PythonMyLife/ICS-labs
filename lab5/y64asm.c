#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "y64asm.h"

line_t *line_head = NULL;
line_t *line_tail = NULL;
int lineno = 0;

#define err_print(_s, _a ...) do { \
  if (lineno < 0) \
    fprintf(stderr, "[--]: "_s"\n", ## _a); \
  else \
    fprintf(stderr, "[L%d]: "_s"\n", lineno, ## _a); \
} while (0);


int64_t vmaddr = 0;    /* vm addr */

/* register table */
const reg_t reg_table[REG_NONE] = {
    {"%rax", REG_RAX, 4},
    {"%rcx", REG_RCX, 4},
    {"%rdx", REG_RDX, 4},
    {"%rbx", REG_RBX, 4},
    {"%rsp", REG_RSP, 4},
    {"%rbp", REG_RBP, 4},
    {"%rsi", REG_RSI, 4},
    {"%rdi", REG_RDI, 4},
    {"%r8",  REG_R8,  3},
    {"%r9",  REG_R9,  3},
    {"%r10", REG_R10, 4},
    {"%r11", REG_R11, 4},
    {"%r12", REG_R12, 4},
    {"%r13", REG_R13, 4},
    {"%r14", REG_R14, 4}
};
const reg_t* find_register(char *name)
{
    int i;
    for (i = 0; i < REG_NONE; i++)
        if (!strncmp(name, reg_table[i].name, reg_table[i].namelen))
            return &reg_table[i];
    return NULL;
}


/* instruction set */
instr_t instr_set[] = {
    {"nop", 3,   HPACK(I_NOP, F_NONE), 1 },
    {"halt", 4,  HPACK(I_HALT, F_NONE), 1 },
    {"rrmovq", 6,HPACK(I_RRMOVQ, F_NONE), 2 },
    {"cmovle", 6,HPACK(I_RRMOVQ, C_LE), 2 },
    {"cmovl", 5, HPACK(I_RRMOVQ, C_L), 2 },
    {"cmove", 5, HPACK(I_RRMOVQ, C_E), 2 },
    {"cmovne", 6,HPACK(I_RRMOVQ, C_NE), 2 },
    {"cmovge", 6,HPACK(I_RRMOVQ, C_GE), 2 },
    {"cmovg", 5, HPACK(I_RRMOVQ, C_G), 2 },
    {"irmovq", 6,HPACK(I_IRMOVQ, F_NONE), 10 },
    {"rmmovq", 6,HPACK(I_RMMOVQ, F_NONE), 10 },
    {"mrmovq", 6,HPACK(I_MRMOVQ, F_NONE), 10 },
    {"addq", 4,  HPACK(I_ALU, A_ADD), 2 },
    {"subq", 4,  HPACK(I_ALU, A_SUB), 2 },
    {"andq", 4,  HPACK(I_ALU, A_AND), 2 },
    {"xorq", 4,  HPACK(I_ALU, A_XOR), 2 },
    {"jmp", 3,   HPACK(I_JMP, C_YES), 9 },
    {"jle", 3,   HPACK(I_JMP, C_LE), 9 },
    {"jl", 2,    HPACK(I_JMP, C_L), 9 },
    {"je", 2,    HPACK(I_JMP, C_E), 9 },
    {"jne", 3,   HPACK(I_JMP, C_NE), 9 },
    {"jge", 3,   HPACK(I_JMP, C_GE), 9 },
    {"jg", 2,    HPACK(I_JMP, C_G), 9 },
    {"call", 4,  HPACK(I_CALL, F_NONE), 9 },
    {"ret", 3,   HPACK(I_RET, F_NONE), 1 },
    {"pushq", 5, HPACK(I_PUSHQ, F_NONE), 2 },
    {"popq", 4,  HPACK(I_POPQ, F_NONE),  2 },
    {".byte", 5, HPACK(I_DIRECTIVE, D_DATA), 1 },
    {".word", 5, HPACK(I_DIRECTIVE, D_DATA), 2 },
    {".long", 5, HPACK(I_DIRECTIVE, D_DATA), 4 },
    {".quad", 5, HPACK(I_DIRECTIVE, D_DATA), 8 },
    {".pos", 4,  HPACK(I_DIRECTIVE, D_POS), 0 },
    {".align", 6,HPACK(I_DIRECTIVE, D_ALIGN), 0 },
    {NULL, 1,    0   , 0 } //end
};

instr_t *find_instr(char *name)
{
    int i;
    for (i = 0; instr_set[i].name; i++)
	if (strncmp(instr_set[i].name, name, instr_set[i].len) == 0)
	    return &instr_set[i];
    return NULL;
}

/* symbol table (don't forget to init and finit it) */
symbol_t *symtab = NULL;

/*
 * find_symbol: scan table to find the symbol
 * args
 *     name: the name of symbol
 *
 * return
 *     symbol_t: the 'name' symbol
 *     NULL: not exist
 */
symbol_t *find_symbol(char *name)
{
    symbol_t *symtab_next = symtab->next;
    while(symtab_next != NULL)
    {
        if(strcmp(symtab_next->name,name)==0)
            return symtab_next;
        symtab_next = symtab_next->next;
    }
    return NULL;
}

/*
 * add_symbol: add a new symbol to the symbol table
 * args
 *     name: the name of symbol
 *
 * return
 *     0: success
 *     -1: error, the symbol has exist
 */
int add_symbol(char *name)
{
    /* check duplicate */
    if (find_symbol(name) != NULL)
        return -1;

    /* create new symbol_t (don't forget to free it)*/
    symbol_t *new_symtab = (symbol_t *)malloc(sizeof(symbol_t));
    new_symtab->name = name;
    new_symtab->addr = vmaddr;

    /* add the new symbol_t to symbol table */
    new_symtab->next = symtab->next;
    symtab->next = new_symtab;

    return 0;
}

/* relocation table (don't forget to init and finit it) */
reloc_t *reltab = NULL;

/*
 * add_reloc: add a new relocation to the relocation table
 * args
 *     name: the name of symbol
 *
 * return
 *     0: success
 *     -1: error, the symbol has exist
 */
void add_reloc(char *name, bin_t *bin)
{
    /* create new reloc_t (don't forget to free it)*/
    reloc_t *new_reloc = (reloc_t *)malloc(sizeof(reloc_t));
    new_reloc->y64bin = bin;
    new_reloc->name = name;

    /* add the new reloc_t to relocation table */
    new_reloc->next = reltab->next;
    reltab->next = new_reloc;

}


/* macro for parsing y64 assembly code */
#define IS_DIGIT(s) ((*(s)>='0' && *(s)<='9') || *(s)=='-' || *(s)=='+')
#define IS_LETTER(s) ((*(s)>='a' && *(s)<='z') || (*(s)>='A' && *(s)<='Z'))
#define IS_COMMENT(s) (*(s)=='#')
#define IS_REG(s) (*(s)=='%')
#define IS_IMM(s) (*(s)=='$')

#define IS_BLANK(s) (*(s)==' ' || *(s)=='\t')
#define IS_END(s) (*(s)=='\0')

#define IS_HEX(s) ((*(s) == '0' && *((s) + 1) == 'x') || \
 (*(s) == '-' && *((s) + 1) == '0' && *((s) + 2) == 'x'))

#define SKIP_BLANK(s) do {  \
  while(!IS_END(s) && IS_BLANK(s))  \
    (s)++;    \
} while(0);

/* return value from different parse_xxx function */
typedef enum { PARSE_ERR=-1, PARSE_REG, PARSE_DIGIT, PARSE_SYMBOL, 
    PARSE_MEM, PARSE_DELIM, PARSE_INSTR, PARSE_LABEL} parse_t;

/*
 * parse_instr: parse an expected data token (e.g., 'rrmovq')
 * args
 *     ptr: point to the start of string
 *     inst: point to the inst_t within instr_set
 *
 * return
 *     PARSE_INSTR: success, move 'ptr' to the first char after token,
 *                            and store the pointer of the instruction to 'inst'
 *     PARSE_ERR: error, the value of 'ptr' and 'inst' are undefined
 */
parse_t parse_instr(char **ptr, instr_t **inst)
{
    char *func = *ptr;
    instr_t *instrction;

    /* skip the blank */
    SKIP_BLANK(func);

    /* find_instr and check end */
    if (IS_END(func))
        return PARSE_ERR;

    instrction = find_instr(func);
    if (instrction == NULL)
        return PARSE_ERR;
    
    func += instrction->len;
    if (!IS_END(func) && !IS_BLANK(func))
        return PARSE_ERR;
    
    /* set 'ptr' and 'inst' */
    *ptr = func;
    *inst = instrction;

    return PARSE_INSTR;
}

/*
 * parse_delim: parse an expected delimiter token (e.g., ',')
 * args
 *     ptr: point to the start of string
 *
 * return
 *     PARSE_DELIM: success, move 'ptr' to the first char after token
 *     PARSE_ERR: error, the value of 'ptr' and 'delim' are undefined
 */
parse_t parse_delim(char **ptr, char delim)
{
    char *delimiter = *ptr;

    /* skip the blank and check */
    SKIP_BLANK(delimiter);
    if ((IS_END(delimiter)) || (*delimiter != delim))
        return PARSE_ERR;

    /* set 'ptr' */
    *ptr = delimiter + 1;

    return PARSE_DELIM;
}

/*
 * parse_reg: parse an expected register token (e.g., '%rax')
 * args
 *     ptr: point to the start of string
 *     regid: point to the regid of register
 *
 * return
 *     PARSE_REG: success, move 'ptr' to the first char after token, 
 *                         and store the regid to 'regid'
 *     PARSE_ERR: error, the value of 'ptr' and 'regid' are undefined
 */
parse_t parse_reg(char **ptr, regid_t *regid)
{
    char *regist = *ptr;
    /* skip the blank and check */
    SKIP_BLANK(regist);
    if (IS_END(regist) || !IS_REG(regist)) 
        return PARSE_ERR;

    /* find register */
    const reg_t *p_regist = find_register(regist);
    if (p_regist == NULL)
        return PARSE_ERR;
    regist += p_regist->namelen;

    /* set 'ptr' and 'regid' */
    *ptr = regist;
    *regid = p_regist->id;

    return PARSE_REG;
}

/*
 * parse_symbol: parse an expected symbol token (e.g., 'Main')
 * args
 *     ptr: point to the start of string
 *     name: point to the name of symbol (should be allocated in this function)
 *
 * return
 *     PARSE_SYMBOL: success, move 'ptr' to the first char after token,
 *                               and allocate and store name to 'name'
 *     PARSE_ERR: error, the value of 'ptr' and 'name' are undefined
 */
parse_t parse_symbol(char **ptr, char **name)
{
    char *symbol = *ptr;
    char *name_of_symbol;
    int n = 0;

    /* skip the blank and check */
    SKIP_BLANK(symbol);
    if (IS_END(symbol) || !IS_LETTER(symbol))
        return PARSE_ERR;

    /* allocate name and copy to it */
    while(IS_LETTER(symbol + n) || IS_DIGIT(symbol + n))
    {
        n++;
    }
    name_of_symbol = (char *)malloc(n+1);
    for(int i = 0;i < n;i++)
        name_of_symbol[i] = symbol[i];
    name_of_symbol[n] = '\0';


    /* set 'ptr' and 'name' */
    *ptr = symbol + n;
    *name = name_of_symbol;

    return PARSE_SYMBOL;
}

/*
 * parse_digit: parse an expected digit token (e.g., '0x100')
 * args
 *     ptr: point to the start of string
 *     value: point to the value of digit
 *
 * return
 *     PARSE_DIGIT: success, move 'ptr' to the first char after token
 *                            and store the value of digit to 'value'
 *     PARSE_ERR: error, the value of 'ptr' and 'value' are undefined
 */
parse_t parse_digit(char **ptr, long *value)
{
    char *digit = *ptr;
    long val;

    /* skip the blank and check */
    SKIP_BLANK(digit);
    if (IS_END(digit) || !IS_DIGIT(digit))
        return PARSE_ERR;

    /* calculate the digit, (NOTE: see strtoll())no! ff will explode you */
    int n = IS_HEX(digit)?16:10;
    char *after_digit;
    val = (long)strtoull(digit,&after_digit,n);   

    /* set 'ptr' and 'value' */
    *ptr = after_digit;
    *value = val;

    return PARSE_DIGIT;

}

/*
 * parse_imm: parse an expected immediate token (e.g., '$0x100' or 'STACK')
 * args
 *     ptr: point to the start of string
 *     name: point to the name of symbol (should be allocated in this function)
 *     value: point to the value of digit
 *
 * return
 *     PARSE_DIGIT: success, the immediate token is a digit,
 *                            move 'ptr' to the first char after token,
 *                            and store the value of digit to 'value'
 *     PARSE_SYMBOL: success, the immediate token is a symbol,
 *                            move 'ptr' to the first char after token,
 *                            and allocate and store name to 'name' 
 *     PARSE_ERR: error, the value of 'ptr', 'name' and 'value' are undefined
 */
parse_t parse_imm(char **ptr, char **name, long *value)
{
    char *imm = *ptr;

    char *tmp;
    int n;
    long val = *value;
    char *name_tmp = *name;

    int flag = 0;

    /* skip the blank and check */
    SKIP_BLANK(imm);
    if (IS_END(imm))
        return PARSE_ERR;

    /* if IS_IMM, then parse the digit */
    if (IS_IMM(imm))
    {
        imm++;
        if (!IS_DIGIT(imm))
            return PARSE_ERR;
        n = IS_HEX(imm)?16:10;
        val = strtoll(imm,&tmp,n);
        imm = tmp;
        flag = 1;
    }

    /* if IS_LETTER, then parse the symbol */
    else if (IS_LETTER(imm))
    {
        n = 0;
        while(IS_LETTER(imm+n) || IS_DIGIT(imm+n)) n++;
        tmp = (char *)malloc(n+1);
        for(int i = 0;i < n;i++)
            tmp[i] = imm[i];
        tmp[n] = '\0';
        name_tmp = tmp;
        imm = imm + n;
        flag = 2; 
    }
    
    /* set 'ptr' and 'name' or 'value' */
    *ptr = imm;
    *name = name_tmp;
    *value = val;

    if (flag == 1) return PARSE_DIGIT;
    if (flag == 2) return PARSE_SYMBOL;
    return PARSE_ERR;
}

/*
 * parse_mem: parse an expected memory token (e.g., '8(%rbp)')
 * args
 *     ptr: point to the start of string
 *     value: point to the value of digit
 *     regid: point to the regid of register
 *
 * return
 *     PARSE_MEM: success, move 'ptr' to the first char after token,
 *                          and store the value of digit to 'value',
 *                          and store the regid to 'regid'
 *     PARSE_ERR: error, the value of 'ptr', 'value' and 'regid' are undefined
 */
parse_t parse_mem(char **ptr, long *value, regid_t *regid)
{
    char *mem = *ptr;
    char *after_mem;
    long val = 0;
    int n;

    /* skip the blank and check */
    SKIP_BLANK(mem);
    if (IS_END(mem))
        return PARSE_ERR;

    /* calculate the digit and register, (ex: (%rbp) or 8(%rbp)) */
    /* digit */
    if (IS_DIGIT(mem))
    {
        n = IS_HEX(mem)?16:10;
        val = strtoll(mem,&after_mem,n);
        mem = after_mem;
    }

    if(*mem != '(')
        return PARSE_ERR;
    mem++;

    /* register */
    if(!IS_REG(mem))
        return PARSE_ERR;
    
    const reg_t *reg = find_register(mem);
    if(reg == NULL)
        return PARSE_ERR;
    mem += reg->namelen;
    if(*mem != ')')
        return PARSE_ERR;
    mem++;

    /* set 'ptr', 'value' and 'regid' */
    *ptr = mem;
    *value = val;
    *regid = reg->id;

    return PARSE_MEM;
}

/*
 * parse_data: parse an expected data token (e.g., '0x100' or 'array')
 * args
 *     ptr: point to the start of string
 *     name: point to the name of symbol (should be allocated in this function)
 *     value: point to the value of digit
 *
 * return
 *     PARSE_DIGIT: success, data token is a digit,
 *                            and move 'ptr' to the first char after token,
 *                            and store the value of digit to 'value'
 *     PARSE_SYMBOL: success, data token is a symbol,
 *                            and move 'ptr' to the first char after token,
 *                            and allocate and store name to 'name' 
 *     PARSE_ERR: error, the value of 'ptr', 'name' and 'value' are undefined
 */
parse_t parse_data(char **ptr, char **name, long *value)
{
    char *dat = *ptr;
    char *after_dat;
    int n;
    long val = *value;
    char *name_tmp = *name;
    int flag = 0;


    /* skip the blank and check */
    SKIP_BLANK(dat);
    if (IS_END(dat))
        return PARSE_ERR;

    /* if IS_DIGIT, then parse the digit */
    if (IS_DIGIT(dat))
    {
        n = IS_HEX(dat)?16:10;
        unsigned long val1;
        val1 = strtoll(dat,&after_dat,n);
        val = (long)val1;
        dat = after_dat;
        flag = 1;
    }

    /* if IS_LETTER, then parse the symbol */
    else if (IS_LETTER(dat))
    {
        n = 0;
        while(IS_LETTER(dat+n) || IS_DIGIT(dat+n)) n++;
        name_tmp = (char *)malloc(n + 1);
        for(int i = 0;i < n;i++)
            name_tmp[i] = dat[i];
        name_tmp[n] = '\0';
        dat += n;
        flag = 2;
    }

    /* set 'ptr', 'name' and 'value' */
    *ptr = dat;
    *name = name_tmp;
    *value = val;

    if (flag == 1) return PARSE_DIGIT;
    if (flag == 2) return PARSE_SYMBOL;
    return PARSE_ERR;
}

/*
 * parse_label: parse an expected label token (e.g., 'Loop:')
 * args
 *     ptr: point to the start of string
 *     name: point to the name of symbol (should be allocated in this function)
 *
 * return
 *     PARSE_LABEL: success, move 'ptr' to the first char after token
 *                            and allocate and store name to 'name'
 *     PARSE_ERR: error, the value of 'ptr' is undefined
 */
parse_t parse_label(char **ptr, char **name)
{
    char *lab = *ptr;
    int n = 0;
    char *label;

    /* skip the blank and check */
    SKIP_BLANK(lab);
    if (IS_END(lab) || !IS_LETTER(lab))
        return PARSE_ERR;

    /* allocate name and copy to it */
    while(IS_LETTER(lab+n) || IS_DIGIT(lab+n)) n++;
    if (lab[n] != ':')
        return PARSE_ERR;
    
    label = (char *)malloc(n+1);
    for(int i = 0;i < n;i++) label[i] = lab[i];
    label[n] = '\0';
    lab += n + 1;

    /* set 'ptr' and 'name' */
    *ptr = lab;
    *name = label;

    return PARSE_LABEL;
}

/*
 * parse_line: parse a line of y64 code (e.g., 'Loop: mrmovq (%rcx), %rsi')
 * (you could combine above parse_xxx functions to do it)
 * args
 *     line: point to a line_t data with a line of y64 assembly code
 *
 * return
 *     PARSE_XXX: success, fill line_t with assembled y64 code
 *     PARSE_ERR: error, try to print err information (e.g., instr type and line number)
 */
type_t parse_line(line_t *line)
{
    char *lin;
    bin_t *y64bin;
    char *y64asm;

    y64bin = &line->y64bin;
    y64asm = (char *)malloc(sizeof(char)*(strlen(line->y64asm) + 1));
    strcpy(y64asm, line->y64asm);
    lin = y64asm;

    char *label = NULL;
    instr_t *inst = NULL;
    int n;

    /* when finish parse an instruction or lable, we still need to continue check 
    * e.g., 
    *  Loop: mrmovl (%rbp), %rcx
    *           call SUM  #invoke SUM function */
    start:
        /* skip blank and check IS_END */
        SKIP_BLANK(lin);
        if (IS_END(lin))
            goto end;
        /* is a comment ? */
        if (IS_COMMENT(lin))
            goto end;

        /* is a label ? */
        n = parse_label(&lin, &label);
        if (n == PARSE_LABEL) 
        {
            if (add_symbol(label) < 0) 
            {
                line->type = TYPE_ERR;
                err_print("Dup symbol:%s", label);
                goto end;
            }

            /* set type and y64bin */
            line->type = TYPE_INS;
            line->y64bin.addr = vmaddr;

            goto start;
        }

        /* is an instruction ? */
        n = parse_instr(&lin, &inst);
        if (n == PARSE_ERR) 
        {
            line->type = TYPE_ERR;
            err_print("Invalid instr");
            goto end;
        }

        /* set type and y64bin */
        line->type = TYPE_INS;
        y64bin->addr = vmaddr;
        y64bin->codes[0] = inst->code;
        y64bin->bytes = inst->bytes;

        /* update vmaddr */    
        vmaddr += inst->bytes;

        /* parse the rest of instruction according to the itype */
        regid_t regA, regB;
        switch (HIGH(inst->code)) {
            case I_HALT:  
            case I_NOP:   
            case I_RET: 
                goto start;
            case I_PUSHQ: 
            case I_POPQ: {
                n = parse_reg(&lin, &regA);
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid REG");
                    goto end; 
                }
                    
                y64bin->codes[1] = HPACK(regA, 0xF);
                goto start;
            }
        
            case I_RRMOVQ:
            case I_ALU: { 
                n = parse_reg(&lin, &regA);
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid REG");
                    goto end; 
                }
                        
                n = parse_delim(&lin, ',');
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid ','");
                    goto end; 
                }

                n = parse_reg(&lin, &regB);
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid REG");
                    goto end; 
                }

                y64bin->codes[1] = HPACK(regA, regB);
                goto start;
            }
            
            case I_IRMOVQ: { 
                char *name;
                long value;
                n = parse_imm(&lin, &name, &value);
                if(n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid Immediate");
                    goto end; 
                }
                else if(n == PARSE_SYMBOL) 
                { 
                    add_reloc(name, y64bin); 
                    value = 0;
                }
                long *p = (long *)&y64bin->codes[2];
                *p = value;
                
                n = parse_delim(&lin, ',');
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid ','");
                    goto end; 
                }

                n = parse_reg(&lin, &regB);
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid REG");
                    goto end; 
                }
                y64bin->codes[1] = HPACK(0xF, regB);
                goto start;
            }
            
            case I_RMMOVQ: { 
                long value;
                n = parse_reg(&lin, &regA);
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid REG");
                    goto end; 
                }

                n = parse_delim(&lin, ',');
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid ','");
                    goto end; 
                }

                n = parse_mem(&lin, &value, &regB);
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid MEM");
                    goto end; 
                }

                y64bin->codes[1] = HPACK(regA, regB);
                long *p = (long *)&y64bin->codes[2];
                *p = value;

                goto start;
            }
            
            case I_MRMOVQ: { 
                long value;
                n = parse_mem(&lin, &value, &regB);
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid MEM");
                    goto end; 
                }
                
                n = parse_delim(&lin, ',');
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid ','");
                    goto end; 
                }

                n = parse_reg(&lin, &regA);
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid REG");
                    goto end; 
                }

                y64bin->codes[1] = HPACK(regA, regB);
                long *p = (long *)&y64bin->codes[2];
                *p = value;

                goto start;
            }
            
            case I_JMP:  
            case I_CALL: {
                char *name;
                n = parse_symbol(&lin, &name);
                if (n == PARSE_ERR) 
                {
                    line->type = TYPE_ERR;
                    err_print("Invalid DEST");
                    goto end; 
                }

                add_reloc(name, y64bin);
                goto start;
            }
            
            case I_DIRECTIVE: {
                switch (LOW(inst->code)) {
                    case D_DATA: { 
                        long value;
                        n = parse_digit(&lin, &value);
                        if (n == PARSE_ERR) 
                        {
                            char *name;
                            n = parse_symbol(&lin, &name);
                            if(n == PARSE_ERR) 
                            {
                                line->type = TYPE_ERR;
                                err_print("Invalid DATA");
                                goto end; 
                            }
                            add_reloc(name, y64bin);
                        }
                        long *p = (long *)y64bin->codes;
                        *p = value;

                        goto start;
                        }
                
                    case D_POS: {   
                        long value;
                        n = parse_digit(&lin, &value);
                        if (n == PARSE_ERR) 
                        {
                            line->type = TYPE_ERR;
                            err_print("Invalid POS");
                            goto end; 
                        }
                        
                        vmaddr = value;
                        y64bin->addr = vmaddr;
                        goto start;
                    }
                
                    case D_ALIGN: { 
                        long value;
                        n = parse_digit(&lin, &value);
                        if (n == PARSE_ERR) 
                        {
                            line->type = TYPE_ERR;
                            err_print("Invalid ALIGN");
                            goto end; 
                        }

                        long spr = vmaddr % value;
                        if(spr != 0) vmaddr += value - spr;
                        y64bin->addr = vmaddr;

                        goto start;
                    }
                    default:
                        line->type = TYPE_ERR;
                        err_print("Unknown directive");
                        goto end;
                }
                break;
            }
            default:
                line->type = TYPE_ERR;
                err_print("Unknown instruction");
                goto end;
        }
    
    end:
        free(y64asm);
        return line->type;
}

/*
 * assemble: assemble an y64 file (e.g., 'asum.ys')
 * args
 *     in: point to input file (an y64 assembly file)
 *
 * return
 *     0: success, assmble the y64 file to a list of line_t
 *     -1: error, try to print err information (e.g., instr type and line number)
 */
int assemble(FILE *in)
{
    static char asm_buf[MAX_INSLEN]; /* the current line of asm code */
    line_t *line;
    int slen;
    char *y64asm;

    /* read y64 code line-by-line, and parse them to generate raw y64 binary code list */
    while (fgets(asm_buf, MAX_INSLEN, in) != NULL) {
        slen  = strlen(asm_buf);
        while ((asm_buf[slen-1] == '\n') || (asm_buf[slen-1] == '\r')) { 
            asm_buf[--slen] = '\0'; /* replace terminator */
        }

        /* store y64 assembly code */
        y64asm = (char *)malloc(sizeof(char) * (slen + 1)); // free in finit
        strcpy(y64asm, asm_buf);

        line = (line_t *)malloc(sizeof(line_t)); // free in finit
        memset(line, '\0', sizeof(line_t));

        line->type = TYPE_COMM;
        line->y64asm = y64asm;
        line->next = NULL;

        line_tail->next = line;
        line_tail = line;
        lineno ++;

        if (parse_line(line) == TYPE_ERR) {
            return -1;
        }
    }

	lineno = -1;
    return 0;
}

/*
 * relocate: relocate the raw y64 binary code with symbol address
 *
 * return
 *     0: success
 *     -1: error, try to print err information (e.g., addr and symbol)
 */
int relocate(void)
{
    reloc_t *rtmp = NULL;
    
    rtmp = reltab->next;
    while (rtmp) {
        /* find symbol */
        symbol_t *symb = find_symbol(rtmp->name);
        if (!symb)
        {
            err_print("Unknown symbol:'%s'",rtmp->name);
            return -1;
        }

        /* relocate y64bin according itype */
        long *relo;
        byte_t itype = HIGH(rtmp->y64bin->codes[0]);
        if (itype == I_IRMOVQ)
            relo = (long *)&rtmp->y64bin->codes[2];
        else if ((itype == I_CALL) || (itype == I_JMP))
            relo = (long *)&rtmp->y64bin->codes[1];
        else 
            relo = (long *)&rtmp->y64bin->codes[0];
        *relo = symb->addr;

        /* next */
        rtmp = rtmp->next;
    }
    return 0;
}

/*
 * binfile: generate the y64 binary file
 * args
 *     out: point to output file (an y64 binary file)
 *
 * return
 *     0: success
 *     -1: error
 */
int binfile(FILE *out)
{
    /* prepare image with y64 binary code */
    line_t *output = line_head;
    int n = 0;
    line_t *p;

    /* binary write y64 code to output file (NOTE: see fwrite()) */
    while (output)
    {
        bool_t flag = 1;
        p = output->next;
        while(p)
        {
            if (p->y64bin.bytes != 0)
            {
                flag = 0;
                break;
            }
            p = p->next;
            flag = 1;
        }
        if ((output->y64bin.addr > n) && !flag)
        {
            int m = output->y64bin.addr - n;
            for(int i = 0; i < m; i++)
                fprintf(out,"%c",0);
            n =output->y64bin.addr;
        }
        size_t a = fwrite(output->y64bin.codes,sizeof(byte_t),output->y64bin.bytes,out);
        if (a != output->y64bin.bytes)
            return -1;

        n += output->y64bin.bytes;
        output = output->next; 
    }

    return 0;
}

/* whether print the readable output to screen or not ? */
bool_t screen = FALSE; 

static void hexstuff(char *dest, int value, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        char c;
        int h = (value >> 4*i) & 0xF;
        c = h < 10 ? h + '0' : h - 10 + 'a';
        dest[len-i-1] = c;
    }
}

void print_line(line_t *line)
{
    char buf[32];

    /* line format: 0xHHH: cccccccccccc | <line> */
    if (line->type == TYPE_INS) {
        bin_t *y64bin = &line->y64bin;
        int i;
        
        strcpy(buf, "  0x000:                      | ");
        
        hexstuff(buf+4, y64bin->addr, 3);
        if (y64bin->bytes > 0)
            for (i = 0; i < y64bin->bytes; i++)
                hexstuff(buf+9+2*i, y64bin->codes[i]&0xFF, 2);
    } else {
        strcpy(buf, "                              | ");
    }

    printf("%s%s\n", buf, line->y64asm);
}

/* 
 * print_screen: dump readable binary and assembly code to screen
 * (e.g., Figure 4.8 in ICS book)
 */
void print_screen(void)
{
    line_t *tmp = line_head->next;
    while (tmp != NULL) {
        print_line(tmp);
        tmp = tmp->next;
    }
}

/* init and finit */
void init(void)
{
    reltab = (reloc_t *)malloc(sizeof(reloc_t)); // free in finit
    memset(reltab, 0, sizeof(reloc_t));

    symtab = (symbol_t *)malloc(sizeof(symbol_t)); // free in finit
    memset(symtab, 0, sizeof(symbol_t));

    line_head = (line_t *)malloc(sizeof(line_t)); // free in finit
    memset(line_head, 0, sizeof(line_t));
    line_tail = line_head;
    lineno = 0;
}

void finit(void)
{
    reloc_t *rtmp = NULL;
    do {
        rtmp = reltab->next;
        if (reltab->name) 
            free(reltab->name);
        free(reltab);
        reltab = rtmp;
    } while (reltab);
    
    symbol_t *stmp = NULL;
    do {
        stmp = symtab->next;
        if (symtab->name) 
            free(symtab->name);
        free(symtab);
        symtab = stmp;
    } while (symtab);

    line_t *ltmp = NULL;
    do {
        ltmp = line_head->next;
        if (line_head->y64asm) 
            free(line_head->y64asm);
        free(line_head);
        line_head = ltmp;
    } while (line_head);
}

static void usage(char *pname)
{
    printf("Usage: %s [-v] file.ys\n", pname);
    printf("   -v print the readable output to screen\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    int rootlen;
    char infname[512];
    char outfname[512];
    int nextarg = 1;
    FILE *in = NULL, *out = NULL;
    
    if (argc < 2)
        usage(argv[0]);
    
    if (argv[nextarg][0] == '-') {
        char flag = argv[nextarg][1];
        switch (flag) {
          case 'v':
            screen = TRUE;
            nextarg++;
            break;
          default:
            usage(argv[0]);
        }
    }

    /* parse input file name */
    rootlen = strlen(argv[nextarg])-3;
    /* only support the .ys file */
    if (strcmp(argv[nextarg]+rootlen, ".ys"))
        usage(argv[0]);
    
    if (rootlen > 500) {
        err_print("File name too long");
        exit(1);
    }
 

    /* init */
    init();

    
    /* assemble .ys file */
    strncpy(infname, argv[nextarg], rootlen);
    strcpy(infname+rootlen, ".ys");
    in = fopen(infname, "r");
    if (!in) {
        err_print("Can't open input file '%s'", infname);
        exit(1);
    }
    
    if (assemble(in) < 0) {
        err_print("Assemble y64 code error");
        fclose(in);
        exit(1);
    }
    fclose(in);


    /* relocate binary code */
    if (relocate() < 0) {
        err_print("Relocate binary code error");
        exit(1);
    }


    /* generate .bin file */
    strncpy(outfname, argv[nextarg], rootlen);
    strcpy(outfname+rootlen, ".bin");
    out = fopen(outfname, "wb");
    if (!out) {
        err_print("Can't open output file '%s'", outfname);
        exit(1);
    }

    if (binfile(out) < 0) {
        err_print("Generate binary file error");
        fclose(out);
        exit(1);
    }
    fclose(out);
    
    /* print to screen (.yo file) */
    if (screen)
       print_screen(); 

    /* finit */
    finit();
    return 0;
}


