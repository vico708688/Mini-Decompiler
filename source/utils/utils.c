#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "utils/utils.h"
#include "backend/CFG.h"
#include "backend/graph.h"

char *read_asm_file(char *path)
{
	FILE *fp;
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
	char *buffer = malloc(buffer_len + 1);

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

// void free_operand(Operand* operand)
// {
// 	switch (operand->kind)
// 	{
// 	case OPERAND_IMM:
// 		free_int(operand->integer);
// 		break;

// 	case OPERAND_REG:
// 		free_reg(operand->reg);
// 		break;

// 	case OPERAND_MEM:
// 		free_mem(operand->mem);
// 		break;

// 	default:
// 		break;
// 	}
// 	free(operand);
// }

// void free_int(Int* integer)
// {
// 	free(integer);
// }

// void free_reg(Reg* reg)
// {
// 	free_operand(reg->index);
// 	free(reg);
// }

// void free_mem(Mem* mem)
// {
// 	free_operand(mem->index);
// 	free(mem);
// }

// void free_flag(Flag* flag)
// {
// 	free(flag);
// }

// void free_src(Src* src)
// {
// 	switch (src->kind)
// 	{
// 	case SRC_INT:
// 		free_int(src->integer);
// 		break;

// 	case SRC_REG:
// 		free_reg(src->reg);
// 		break;

// 	case SRC_MEM:
// 		free_mem(src->mem);
// 		break;

// 	case SRC_FLAG:
// 		free_flag(src->flag);
// 		break;

// 	default:
// 		break;
// 	}
// 	free(src);
// }

// void free_dst(Dst* dst)
// {
// 	switch (dst->kind)
// 	{
// 	case DST_REG:
// 		free_reg(dst->reg);
// 		break;

// 	case DST_MEM:
// 		free_mem(dst->mem);
// 		break;

// 	case DST_FLAG:
// 		free_flag(dst->flag);
// 		break;

// 	default:
// 		break;
// 	}
// 	free(dst);
// }

// void free_load(Load* load)
// {
// 	free_dst(load->dst);
// 	free_src(load->src);
// 	free(load);
// }

// void free_store(Store* store)
// {
// 	free_dst(store->dst);
// 	free_src(store->src);
// 	free(store);
// }

// void free_comparison(Comparison* comparison)
// {
// 	free_dst(comparison->dst);
// 	free_src(comparison->src);
// 	free(comparison);
// }

// void free_operation(Operation* operation)
// {
// 	free_dst(operation->dst);
// 	free_src(operation->src);
// 	free(operation);
// }

// void free_call(Call* call)
// {
// 	free(call->name);
// 	free(call);
// }

// void free_jump(Jump* jump)
// {
// 	if (!jump->jump_resolved)
//     {
// 		free(jump->true_branch);
// 		if (jump->condition->kind != COND_TRUE)
// 		{
// 			free(jump->false_branch);
// 		}
//     }

// 	if (jump->condition)
// 	{
// 		free_condition(jump->condition);
// 	}

// 	free(jump);
// }

// void free_condition(Condition* condition)
// {
// 	if (condition->lcond != NULL)
// 	{
// 		free_operation(condition->lcond);
// 	}
// 	if (condition->rcond != NULL)
// 	{
// 		free_operation(condition->rcond);
// 	}

// 	free(condition);
// }

void free_ir(IR *ir)
{
	free(ir->atomic_instructions);
	free(ir);
}

void free_node(Node *node)
{
	// instructions already free'd
	free(node->successors);
	free(node->predecessors);
	free(node->dominator_frontier);
}

void free_edge(Edge *edge)
{
	// nodes start and target already free'd
	free(edge);
}

void free_cfg(Cfg *cfg)
{
	for (int i = 0; i < cfg->nb_nodes; i++)
	{
		free_node(&cfg->nodes[i]);
	}
	free(cfg->nodes);
	free(cfg->edges);
	free(cfg);
}

char *extract_sub_string(char **string, int len)
{
	char *subString = malloc((len + 1) * sizeof(char));
	if (subString == NULL)
	{
		perror("Error malloc\n");
		exit(1);
	}

	strncpy(subString, *string, len);

	subString[len] = '\0';

	return subString;
}