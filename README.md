# TypoLang
C-like compiled programming language for people writing code with frequent typos.

## Language guide

You can read TypoLang language guide [here](GUIDE.md).

## Interoperability with other languages
This language supports saving its syntax tree in format, specified by
[AST file standard](
https://github.com/MeerkatBoss/ast-standard/blob/master/README.md). This allows
reverse compilation to and from TypoLang, using AST file as an intermediate.

## Usage

To use the TypoLang compiler (tlc) you need to:

1. Clone this repo from Github
    ```bash
    $ git clone https://github.com/MeerkatBoss/compiled-language
    $ cd compiled-language
    ```
2. Checkout to `binary_compiler` branch
    ```bash
    $ git checkout binary_compiler
    ```
3. Build `tlc` with GNU Make
    ```bash
    $ make all
    ```
4. Run `tlc` using GNU Make
    ```bash
    $ make run_frontend ARGS="<argument list>"
    $ make run_midend ARGS="<argument list>"
    $ make run_backend ARGS="<argument list>"
    ```

Information about command-line arguments of frontend, mid-end, and backend
compilers can be obtained by passing "--help" or  "-h" argument to them.

## Technical details

### General information

The TypoLang compiler is a combination of three distinct programs, called 
TypoLang frontend, middle-end, and backend compilers. The TypoLang frontend
compiler transforms the input file written in TypoLang into an Abstract Syntax
Tree (AST) and writes it in standard-compliant file format. TypoLang middle-end
compiler performs optimizations of produced syntax tree, collapsing constant
expressions and removing unnecessary operations (such as multiplication by 0 and
1, addition of 0 etc.). TypoLang backend compiler transforms the AST file into
an ELF executable file for x86-64 machines running Linux.

Compliance with AST file standard allows TypoLang middle-end and backend
compilers work with files produced by other standard-compliant frontend
compilers. Additionally, AST file produced by TypoLang frontend compiler can be
optimized and compiled using other standard-compliant middle-end and backend
compilers, which allows for cross-platform compilation.

### Input Tokenization

Before converting the input TypoLang file to Abstract Syntax Tree, the TypoLang
frontend compiler performs its *tokenization*, extracting TypoLang keywords and
operators, variable and function names, and constant values.

### Token Parsing

TypoLang frontend compiler passes the list of produced tokens to *parser*, which
converts it into AST using the recursive descent algorithm. The produced AST
is written into output file in a standard-compliant format.

### Middle-end Optimizations

TypoLang middle-end compiler reads the AST from file produced by frontend and
analyzes it, finding patterns which can be optimized. The optimized tree is then
written using the same format into an output file.

### Backend Intermediate Representation

Before producing x86-64 bytecode, TypoLang backend compiler converts the AST
into a linked list of entries. This list is called *Intermediate Representation
(IR)*. Usage of IR allows the backend compiler to efficiently calculate target
offsets of `JMP`, `Jcc` and `CALL` instructions. The IR also allows future
optimizations of bytecode based on analysis of nearby IR entries.

### ELF files

ELF (Executable and Linking Format) requires:

1. *ELF file header*, containing general information about file:
```c
#define EI_NIDENT 16
typedef struct {
    unsigned char e_ident[EI_NIDENT];   // Magic constants and ABI version
    uint16_t      e_type;               // File type (ET_EXEC)
    uint16_t      e_machine;            // Machine info (EM_X86_64)
    uint32_t      e_version;            // ELF version (EV_CURRENT)
    Elf64_Addr    e_entry;              // Entry address (for executable files)
    Elf64_Off     e_phoff;              // File offset of first program header
    Elf64_Off     e_shoff;              // File offset of first section header
    uint32_t      e_flags;              // Processor-specific flags (undefined)
    uint16_t      e_ehsize;             // Size of ELF header
    uint16_t      e_phentsize;          // Size of program headers
    uint16_t      e_phnum;              // Number of program headers
    uint16_t      e_shentsize;          // Size of section headers
    uint16_t      e_shnum;              // Number of program headers
    uint16_t      e_shstrndx;           // Index of section, containing section
                                        // names (.strtab)
} Elf64_Ehdr;
```

2. One or more *program headers*, describing memory segments:
```c
typedef struct {
    uint32_t   p_type;      // Segment type
    uint32_t   p_flags;     // Segment flags (Read/Write/Execute)
    Elf64_Off  p_offset;    // Segment start file offset
    Elf64_Addr p_vaddr;     // Segment start virtual address
    Elf64_Addr p_paddr;     // Segment start physical address (equal to virtual
                            // on Linux)
    uint64_t   p_filesz;    // Size of segment in file
    uint64_t   p_memsz;     // Size of segment in memory
    uint64_t   p_align;     // Segment alignment (must be a multiple of page
                            // size)
} Elf64_Phdr;
```

3. Zero or more *section headers*, describing sections serving different
    purpose:
```c
typedef struct {
    uint32_t   sh_name;     // Index of section name in .strtab
    uint32_t   sh_type;     // Section type
    uint64_t   sh_flags;    // Section flags (Read, Alloc, Execute)
    Elf64_Addr sh_addr;     // Section address in memory
    Elf64_Off  sh_offset;   // Section start file offset
    uint64_t   sh_size;     // Size of section
    uint32_t   sh_link;     // Helper field
    uint32_t   sh_info;     // Helper field
    uint64_t   sh_addralign;// Alignment requirement
    uint64_t   sh_entsize;  // Size of entry (for sections containing entries
                            // of fixed size)
} Elf64_Shdr;
```

The ELF file produced by TypoLang backend compiler contains two segments of type
LOAD, which are loaded into memory before execution. The first segment can be
read and executed and contains binary code. The second one can be read from and
written to and contains space reserved for global variables.

Additionally, the produced file contains four sections. The first one is empty
and is required by ELF standard. The second one is the `.text` section,
containing executable code. This section is contained within the first LOAD
segment. Next is the `.bss` section, which occupies no actual space on disk, but
its memory image has the size enough to contain all global variables and is
filled with zeros upon loading. This section is contained within the second LOAD
segment. The last section is `.strtab` - string table which contains names of
all listed sections.
