// assembler_generator.h

#ifndef ASSEMBLER_GENERATOR_H
#define ASSEMBLER_GENERATOR_H

#include "../tac/tac_generator.h"

void ass_gen(instruction_list* list, FILE* ass_out);
char* instruction_to_assembler(instruction i, char** data_ptr, char** text_ptr);

#endif
