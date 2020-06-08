#include <elf.h>
#include <system.h>

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