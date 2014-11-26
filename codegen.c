/*
Justin Canton, 1000017910
Samprit Raihan, 998138830
*/
#include "codegen.h"

int tempCount = 0;
int tempDeclared = 0;
int argumentsCount = 0;
int constructfunc = 0;
int ifFlag = 0;
int ifMax = 0;

FILE *filePointer = fopen("./frag.txt", "w");

int generateAssembly(node * ast){
	int s;
	int i;
	// Open the assembly file
	
	switch(ast->kind) {
  
	    case SCOPE_NODE:
	    	printf("SCOPE\n");
	    	if(ast->scope_expr.left != NULL){
	    		generateAssembly(ast->scope_expr.left);
	    	}
	    	if(ast->scope_expr.right != NULL){
	    		generateAssembly(ast->scope_expr.right);
	    	}
	      break;

	    case BINARY_EXPRESSION_NODE:
	      	break;

	    case UNARY_EXPRESSION_NODE:
	      	break;

	    case INT_NODE:
	    	//fprintf(filePointer, "#INT\n");
	    	fprintf(filePointer, "%d", ast->int_expr.val);
	      	break;

	    case FLOAT_NODE:
	    	//fprintf(filePointer, "#FLOAT\n");
	    	fprintf(filePointer, "%f", ast->float_expr.val);
	     	break;

	    case BOOL_NODE:
	    	if(ast->bool_expr.val == 263)
	    		fprintf(filePointer, "1");
	    	if(ast->bool_expr.val == 262)
	    		fprintf(filePointer, "-1");
	      	break;

	    case VAR_NODE:
	    	printf("VAR\n");
	    	if(ast->var_expr.arr == -1){
	    		fprintf(filePointer, "%s", ast->var_expr.id);
	    	}else if(ast->var_expr.arr == 0){
	    		fprintf(filePointer, "%s.x", ast->var_expr.id);
	    	}else if(ast->var_expr.arr == 1){
	    		fprintf(filePointer, "%s.y", ast->var_expr.id);
	    	}else if(ast->var_expr.arr == 2){
	    		fprintf(filePointer, "%s.z", ast->var_expr.id);
	    	}else if(ast->var_expr.arr == 3){
	    		fprintf(filePointer, "%s.w", ast->var_expr.id);
	    	}
	      	break;

	    case DECLARATION_NODE:
	    	fprintf(filePointer, "#DECLARATION\n");
	    	if(ast->declaration_expr.constant == 261){
	    		if(ast->declaration_expr.right->kind == BINARY_EXPRESSION_NODE){
	    			generateAssembly(ast->declaration_expr.right);
	    			fprintf(filePointer, "PARAM %s = tempVar%d;\n", ast->declaration_expr.id, tempCount);
	    		}else{
	    			fprintf(filePointer, "PARAM %s = ",  ast->declaration_expr.id);
	    			generateAssembly(ast->declaration_expr.right);
	    			fprintf(filePointer, ";\n");
	    		}
	    	}else{
	    		if(ast->declaration_expr.right == NULL){
	    			fprintf(filePointer, "TEMP %s;\n", ast->declaration_expr.id);
	    		}else{
	    			fprintf(filePointer, "TEMP %s;\n",  ast->declaration_expr.id);
	    			if(ast->declaration_expr.right->kind == BINARY_EXPRESSION_NODE || 
	    				ast->declaration_expr.right->kind == UNARY_EXPRESSION_NODE ||
	    				ast->declaration_expr.right->kind == CONSTRUCTOR_NODE ||
	    				ast->declaration_expr.right->kind == FUNCTION_NODE){
	    				s = generateAssembly(ast->declaration_expr.right);
	    				fprintf(filePointer, "MOV %s, tempVar%d;\n",  ast->declaration_expr.id, s);
	    			}else{
	    				fprintf(filePointer, "MOV %s, ",  ast->declaration_expr.id);
	    				generateAssembly(ast->declaration_expr.right);
	    				fprintf(filePointer, ";\n");
	    			}
	    		}
	    	}
	      	break;

	    case IF_STATEMENT_NODE:
	    	if(ifFlag >= ifMax){
	    		ifMax += 1;
	    		ifFlag += 1;
	    		fprintf(filePointer, "TEMP condVar%d;\n", ifFlag);
	    	}else{
	    		ifFlag += 1;
	    	}
	    	if(ast->if_expr.if_comparison != NULL){
				if(ast->if_expr.if_comparison->kind == BINARY_EXPRESSION_NODE || 
					ast->if_expr.if_comparison->kind == UNARY_EXPRESSION_NODE ||
					ast->if_expr.if_comparison->kind == CONSTRUCTOR_NODE ||
					ast->if_expr.if_comparison->kind == FUNCTION_NODE){
					s = generateAssembly(ast->if_expr.if_comparison);
					fprintf(filePointer, "CMP condVar%d, tempVar%d;\n", ifFlag, s);
				}else{
					fprintf(filePointer, "MOV condVar%d, ", ifFlag);
					generateAssembly(ast->if_expr.if_comparison);
					fprintf(filePointer, ";\n");
				}
			}
			if(ast->if_expr.if_statement != NULL)
				generateAssembly(ast->if_expr.if_statement);
			fprintf(filePointer, "SUB condVar%d, 0, condVar%d;\n", ifFlag, ifFlag);
			if(ast->if_expr.else_statement != NULL)
				generateAssembly(ast->if_expr.else_statement);
	    	if(ifFlag >= ifMax)
	    		ifFlag -= 1;
	      	break;

	    case STATEMENT_NODE:
	    	printf("STATEMENTS\n");
	    	if(ast->statement_expr.left != NULL){
	    		generateAssembly(ast->statement_expr.left);
	    	}
	    	if(ast->statement_expr.right != NULL){
	    		generateAssembly(ast->statement_expr.right);
	    	}
	     	break;

	    case ASSIGNMENT_NODE:
	    	if(ifFlag <= 0){
		    	if(ast->assign_expr.right->kind == BINARY_EXPRESSION_NODE || 
					ast->assign_expr.right->kind == UNARY_EXPRESSION_NODE ||
					ast->assign_expr.right->kind == CONSTRUCTOR_NODE ||
					ast->assign_expr.right->kind == FUNCTION_NODE){
					s = generateAssembly(ast->assign_expr.right);
					fprintf(filePointer, "MOV ");
					generateAssembly(ast->assign_expr.left);
					fprintf(filePointer, ", tempVar%d;\n", s);
				}else{
					fprintf(filePointer, "MOV ");
					generateAssembly(ast->assign_expr.left);
					fprintf(filePointer, ", ");
					generateAssembly(ast->assign_expr.right);
					fprintf(filePointer, ";\n");
				}
				return s;
			}else{
				if(ast->assign_expr.right->kind == BINARY_EXPRESSION_NODE || 
					ast->assign_expr.right->kind == UNARY_EXPRESSION_NODE ||
					ast->assign_expr.right->kind == CONSTRUCTOR_NODE ||
					ast->assign_expr.right->kind == FUNCTION_NODE){
					s = generateAssembly(ast->assign_expr.right);
					for(i = ifFlag; i >= 1; i--){
						if(i == 1){
							fprintf(filePointer, "CMP ");
							generateAssembly(ast->assign_expr.left);
							fprintf(filePointer, ", condVar%d, ", i);
							generateAssembly(ast->assign_expr.left);
							fprintf(filePointer,", tempVar%d;\n", s);
						}else{
							fprintf(filePointer, "CMP ");
							fprintf(filePointer, "tempVar%d, condVar%d, ", s, i);
							generateAssembly(ast->assign_expr.left);
							fprintf(filePointer,", tempVar%d;\n", s);
						}
					}
				}else{
					if(tempCount >= tempDeclared){
			    		tempDeclared += 1;
			    		tempCount += 1;
			    		fprintf(filePointer, "TEMP tempVar%d;\n", tempCount);
			    		s = tempCount;
			    	}else{
			    		tempCount += 1;
			    		s = tempCount;
			    	}
					for(i = ifFlag; i >= 1; i--){
						if(i == 1){
							fprintf(filePointer, "CMP ");
							generateAssembly(ast->assign_expr.left);
							fprintf(filePointer, ", condVar%d, ", i);
							generateAssembly(ast->assign_expr.left);
							fprintf(filePointer, ", ");
							generateAssembly(ast->assign_expr.right);
							fprintf(filePointer, ";\n");
						}else{
							fprintf(filePointer, "CMP ");
							fprintf(filePointer, "tempVar%d, condVar%d, ", s, i);
							generateAssembly(ast->assign_expr.left);
							fprintf(filePointer, ", ");
							generateAssembly(ast->assign_expr.right);
							fprintf(filePointer, ";\n");
						}
					}
					if(tempCount >= tempDeclared)
	    				tempCount -= 1;
				}
				return s;
			}
	      	break;

	    case FUNCTION_NODE:
	    	if(tempCount >= tempDeclared){
	    		tempDeclared += 1;
	    		tempCount += 1;
	    		fprintf(filePointer, "TEMP tempVar%d;\n", tempCount);
	    		s = tempCount;
	    	}else{
	    		tempCount += 1;
	    		s = tempCount;
	    	}
	    	constructfunc = 2;
	    	if(ast->func_expr.func == 264){
	    		fprintf(filePointer, "DP3 tempVar%d", tempCount);
	    		generateAssembly(ast->construt_expr.right);
	    		fprintf(filePointer, ";\n");
	    	}else if(ast->func_expr.func == 265){
	    		fprintf(filePointer, "LIT tempVar%d", tempCount);
	    		generateAssembly(ast->construt_expr.right);
	    		fprintf(filePointer, ";\n");
	    	}else if(ast->func_expr.func == 266){
	    		fprintf(filePointer, "RSQ tempVar%d", tempCount);
	    		generateAssembly(ast->construt_expr.right);
	    		fprintf(filePointer, ";\n");
	    	}
	    	if(tempCount >= tempDeclared)
	    		tempCount -= 1;
	    	return s;
	      	break;

	    case CONSTRUCTOR_NODE:
	    	if(tempCount >= tempDeclared){
	    		tempDeclared += 1;
	    		tempCount += 1;
	    		fprintf(filePointer, "TEMP tempVar%d;\n", tempCount);
	    		s = tempCount;
	    	}else{
	    		tempCount += 1;
	    		s = tempCount;
	    	}
	    	constructfunc = 1;
	    	generateAssembly(ast->construt_expr.right);
	    	if(tempCount >= tempDeclared)
	    		tempCount -= 1;
	    	return s;
	      	break;

	    case NESTED_SCOPE_NODE:
	    	printf("NESTED SCOPE\n");
	    	if(ast->nest_scope_expr.scope != NULL){
	    		generateAssembly(ast->nest_scope_expr.scope);
	    	}
	      	break;

	    case DECLARATIONS_NODE:
	    	printf("DECLARATIONS\n");
	    	if(ast->declarations_expr.left != NULL){
	    		generateAssembly(ast->declarations_expr.left);
	    	}
	    	if(ast->declarations_expr.right != NULL){
	    		generateAssembly(ast->declarations_expr.right);
	    	}
	      	break;

	    case ARGUMENTS_NODE:
	    	printf("ARGUMENTS\n");
	    	if(ast->arguments_expr.left != NULL)
	    		generateAssembly(ast->arguments_expr.left);
	    	if(argumentsCount == 0 && constructfunc == 1){
	    		fprintf(filePointer, "MOV tempVar%d.x, ", tempCount);
	    		generateAssembly(ast->arguments_expr.right);
	    		argumentsCount += 1;
	    		fprintf(filePointer, ";\n");
	    	}else if(argumentsCount == 1 && constructfunc == 1){
	    		fprintf(filePointer, "MOV tempVar%d.y, ", tempCount);
	    		generateAssembly(ast->arguments_expr.right);
	    		argumentsCount += 1;
	    		fprintf(filePointer, ";\n");
	    	}else if(argumentsCount == 2 && constructfunc == 1){
	    		fprintf(filePointer, "MOV tempVar%d.z, ", tempCount);
	    		generateAssembly(ast->arguments_expr.right);
	    		argumentsCount += 1;
	    		fprintf(filePointer, ";\n");
	    	}else if(argumentsCount == 3 && constructfunc == 1){
	    		fprintf(filePointer, "MOV tempVar%d.w, ", tempCount);
	    		generateAssembly(ast->arguments_expr.right);
	    		argumentsCount += 1;
	    		fprintf(filePointer, ";\n");
	    	}else if(argumentsCount == 0 && constructfunc == 2){
	    		fprintf(filePointer, ", ");
	    		generateAssembly(ast->arguments_expr.right);
	    		argumentsCount += 1;
	    	}else if(argumentsCount == 1 && constructfunc == 2){
	    		fprintf(filePointer, ", ");
	    		generateAssembly(ast->arguments_expr.right);
	    		argumentsCount += 1;
	    	}
	      	break;

	    case ARGUMENTS_OPT_NODE:
	    	printf("ARGUMENT OPS\n");
	    	generateAssembly(ast->arguments_opt_expr.argum);
	    	argumentsCount = 0;
	      	break;

	    case EXPRESSION_NODE:
	    	if(ast->expression_expr.expr != NULL){
	    		generateAssembly(ast->expression_expr.expr);
	    	}
	      	break;

	    default: break;
	}
	return -1;
	/*
		From this point on, keep adding new instructions
		to the assembly file on a new line using 
		fprint(filePointer, <newline terminated string>)
	*/
}