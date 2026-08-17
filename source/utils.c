#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "CFG.h"
#include "graph.h"

char* read_asm_file(char* path)
{
    FILE* fp;
    int ret;
    struct stat statbuf;

    fp = fopen(path, "r");
    if (fp == NULL)
	{
        fprintf(stderr, "fopen() failed, path: %s\n", path);
        exit(1);
    }
    
    if (stat(path, &statbuf) == -1)
	{
        fprintf(stderr, "fstat() failed\n");
        exit(1);
    }

    int buffer_len = statbuf.st_size;
    char* buffer = malloc(buffer_len + 1);

    if (buffer == NULL)
	{
        fprintf(stderr, "malloc() failed\n");
        exit(1);
    }

    ret = fread(buffer, 1, buffer_len, fp);
    if (ret != buffer_len)
	{
        fprintf(stderr, "fread() failed, ret: %d\n", ret);
        fclose(fp);
        exit(1);
    }

    buffer[buffer_len] = '\0';

    fclose(fp);

    return buffer;
}

void free_tokens(TokenList* tokenList)
{
	for (int i = 0; i < tokenList->nb_tokens; i++)
	{
		if (tokenList->tokens[i].ownstr)
		{
            free(tokenList->tokens[i].value.value_str);
        }
	}
	free(tokenList->tokens);
	free(tokenList);
}

void free_operand(ASMOperand* operand)
{
	switch (operand->kind)
	{
	case OPERAND_INT:
		free_int(operand->integer);
		break;
	
	case OPERAND_REG:
		free_reg(operand->reg);
		break;
	
	case OPERAND_MEM:
		free_mem(operand->mem);
		break;
	
	default:
		break;
	}
	free(operand);
}

void free_int(ASMInt* integer)
{
	free(integer);
}

void free_reg(ASMReg* reg)
{
	free_operand(reg->index);
	free(reg);
}

void free_mem(ASMMem* mem)
{
	free_operand(mem->index);
	free(mem);
}

void free_flag(ASMFlag* flag)
{
	free(flag);
}

void free_src(ASMSrc* src)
{
	switch (src->kind)
	{
	case SRC_INT:
		free_int(src->integer);
		break;
		
	case SRC_REG:
		free_reg(src->reg);
		break;
		
	case SRC_MEM:
		free_mem(src->mem);
		break;
	
	case SRC_FLAG:
		free_flag(src->flag);
		break;
		
	default:
		break;
	}
	free(src);
}

void free_dst(ASMDst* dst)
{
	switch (dst->kind)
	{
	case DST_REG:
		free_reg(dst->reg);
		break;
	
	case DST_MEM:
		free_mem(dst->mem);
		break;
	
	case DST_FLAG:
		free_flag(dst->flag);
		break;
	
	default:
		break;
	}
	free(dst);
}

void free_load(ASMLoad* load)
{
	free_dst(load->dst);
	free_src(load->src);
	free(load);
}

void free_store(ASMStore* store)
{
	free_dst(store->dst);
	free_src(store->src);
	free(store);
}

void free_comparison(ASMComparison* comparison)
{
	free_dst(comparison->dst);
	free_src(comparison->src);
	free(comparison);
}

void free_operation(ASMOperation* operation)
{
	free_dst(operation->dst);
	free_src(operation->src);
	free(operation);
}

void free_call(ASMCall* call)
{
	free(call->name);
	free(call);
}

void free_jump(ASMJump* jump)
{
	if (!jump->jump_resolved)
    {
		free(jump->true_branch);
		if (jump->condition->kind != COND_TRUE)
		{
			free(jump->false_branch);
		}
    }

	if (jump->condition)
	{
		free_condition(jump->condition);
	}
	
	free(jump);
}

void free_condition(ASMCondition* condition)
{
	if (condition->lcond != NULL)
	{
		free_operation(condition->lcond);
	}
	if (condition->rcond != NULL)
	{
		free_operation(condition->rcond);
	}
	
	free(condition);
}

void free_instruction(Instruction* instruction)
{
	switch (instruction->kind)
	{
	case INSTR_LOAD:
		free_load(instruction->load);
		break;
		
	case INSTR_STORE:
		free_store(instruction->store);
		break;
		
		case INSTR_CALL:
		free_call(instruction->call);
		break;
		
		case INSTR_CMP:
		free_comparison(instruction->comparison);
		break;
		
		case INSTR_JMP:
		free_jump(instruction->jump);
		break;
		
		case INSTR_OP:
		free_operation(instruction->operation);
		break;
		
		default:
		break;
	}
}

void free_asm(Asm* program)
{
	for (int i = 0; i < program->nb_instructions; i++)
	{
		Instruction* instruction = &program->instructions[i];
		free_instruction(instruction);
	}
	free(program->instructions);
	free(program);
}

void free_node(Node* node)
{
	// instructions already free'd
	free(node->children);
	free(node->parents);
}

void free_edge(Edge* edge)
{
	// nodes start and target already free'd
	free(edge);
}

void free_cfg(Cfg* cfg)
{
	for (int i = 0; i < cfg->nb_nodes; i++)
	{
		free_node(&cfg->nodes[i]);
	}
	free(cfg->nodes);
	free(cfg->edges);
	free(cfg);
}

void free_regexes(regexList* regexes)
{
	regfree(&(regexes->integer));
	regfree(&(regexes->charac));
}

void init_regexes(regexList* regexes)
{
	if(regcomp(&(regexes->integer), "^[0-9]+", REG_EXTENDED))
	{
		perror("Error regcomp integer\n");
		exit(1);
	}

	if(regcomp(&(regexes->charac), "^[[:alpha:]][a-zA-Z_0-9]*", REG_EXTENDED))
	{
		perror("Error regcomp character\n");
		exit(1);
	}
}

TokenList* init_token_list(char** text)
{
	TokenList *tokenList = malloc(sizeof(TokenList));

	if (tokenList == NULL)
	{
		printf("Error tokenList malloc.\n");
		exit(1);
	}
	
	tokenList->nb_tokens = 256;
	tokenList->token_index = 0;

	if ((tokenList->tokens = calloc(tokenList->nb_tokens, sizeof(Token))) == NULL)
	{
		perror("Error calloc\n");
		exit(1);
	}

	return tokenList;
}

char* extract_sub_string(char** string, int len)
{
	char* subString = malloc((len + 1) * sizeof(char));
	if (subString == NULL)
	{
		perror("Error malloc\n");
		exit(1);
	}
	
	strncpy(subString, *string, len);

	subString[len] = '\0';

	return subString;
}