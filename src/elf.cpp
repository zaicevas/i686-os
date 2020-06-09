#include <elf.h>
#include <system.h>
#include <terminal.h>

using namespace terminal;

bool is_elf_file(Elf32_Ehdr *hdr) {
	if(!hdr) return false;
	if(hdr->e_ident[EI_MAG0] != ELFMAG0) {
		PANIC("ELF Header EI_MAG0 incorrect");
		return false;
	}
	if(hdr->e_ident[EI_MAG1] != ELFMAG1) {
		PANIC("ELF Header EI_MAG1 incorrect");
		return false;
	}
	if(hdr->e_ident[EI_MAG2] != ELFMAG2) {
		PANIC("ELF Header EI_MAG2 incorrect");
		return false;
	}
	if(hdr->e_ident[EI_MAG3] != ELFMAG3) {
		PANIC("ELF Header EI_MAG3 incorrect");
		return false;
	}
	return true;
}

bool is_elf(Elf32_Ehdr *hdr) {
	if(!is_elf_file(hdr)) {
		PANIC("Invalid ELF File");
		return false;
	}
	if(hdr->e_ident[EI_CLASS] != ELFCLASS32) {
		PANIC("Unsupported ELF File Class");
		return false;
	}
	if(hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		PANIC("Unsupported ELF File byte order");
		return false;
	}
	if(hdr->e_machine != EM_386) {
		PANIC("Unsupported ELF File target");
		return false;
	}
	if(hdr->e_ident[EI_VERSION] != EV_CURRENT) {
		PANIC("Unsupported ELF File version");
		return false;
	}
	if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
		PANIC("Unsupported ELF File type");
		return false;
	}
	return true;
}

// static inline Elf32_Shdr *elf_sheader(Elf32_Ehdr *hdr) {
// 	return (Elf32_Shdr *)((int)hdr + hdr->e_shoff);
// }
 
// static inline Elf32_Shdr *elf_section(Elf32_Ehdr *hdr, int idx) {
// 	return &elf_sheader(hdr)[idx];
// }

// static inline char *elf_str_table(Elf32_Ehdr *hdr) {
// 	if(hdr->e_shstrndx == SHN_UNDEF) return nullptr;
// 	return (char *)hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset;
// }
 
// static inline char *elf_lookup_string(Elf32_Ehdr *hdr, int offset) {
// 	char *strtab = elf_str_table(hdr);
// 	if(strtab == nullptr) return nullptr;
// 	return strtab + offset;
// }

// static inline void *elf_load_rel(Elf32_Ehdr *hdr) {
// 	int result;
// 	result = elf_load_stage1(hdr);
// 	if(result == ELF_RELOC_ERR) {
// 		PANIC("Unable to load ELF file.\n");
// 		return nullptr;
// 	}
// 	result = elf_load_stage2(hdr);
// 	if(result == ELF_RELOC_ERR) {
// 		PANIC("Unable to load ELF file.\n");
// 		return nullptr;
// 	}
// 	// TODO : Parse the program header (if present)
// 	return (void *) hdr->e_entry;
// }
 
// void *elf_load_file(void *file) {
// 	Elf32_Ehdr *hdr = (Elf32_Ehdr *)file;
// 	if(!is_elf(hdr)) {
// 		PANIC("ELF File cannot be loaded");
// 		return;
// 	}
// 	switch(hdr->e_type) {
// 		case ET_EXEC:
// 			// TODO : Implement
// 			return nullptr;
// 		case ET_REL:
// 			return elf_load_rel(hdr);
// 	}
// 	return nullptr;
// }

// static int elf_get_symval(Elf32_Ehdr *hdr, int table, uint32_t idx) {
// 	if(table == SHN_UNDEF || idx == SHN_UNDEF) return 0;
// 	Elf32_Shdr *symtab = elf_section(hdr, table);
 
// 	uint32_t symtab_entries = symtab->sh_size / symtab->sh_entsize;
// 	if(idx >= symtab_entries) {
// 		kprintf("Symbol Index out of Range (%d:%u).\n", table, idx);
// 		return ELF_RELOC_ERR;
// 	}
 
// 	int symaddr = (int)hdr + symtab->sh_offset;
// 	Elf32_Sym *symbol = &((Elf32_Sym *)symaddr)[idx];
//     if(symbol->st_shndx == SHN_UNDEF) {
//     // External symbol, lookup value
//     Elf32_Shdr *strtab = elf_section(hdr, symtab->sh_link);
//     const char *name = (const char *)hdr + strtab->sh_offset + symbol->st_name;

//     extern void *elf_lookup_symbol(const char *name);
//     void *target = elf_lookup_symbol(name);

//     if(target == nullptr) {
//         // Extern symbol not found
//         if(ELF32_ST_BIND(symbol->st_info) & STB_WEAK) {
//             // Weak symbol initialized as 0
//             return 0;
//         } else {
//             kprintf("Undefined External Symbol : %s.\n", name);
//             return ELF_RELOC_ERR;
//         }
//     } else {
//         return (int)target;
//     } else if(symbol->st_shndx == SHN_ABS) {
// 		// Absolute symbol
// 		return symbol->st_value;
// 	} else {
// 		// Internally defined symbol
// 		Elf32_Shdr *target = elf_section(hdr, symbol->st_shndx);
// 		return (int)hdr + symbol->st_value + target->sh_offset;
// 	}
// }