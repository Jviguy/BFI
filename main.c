#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <sys/stat.h>
#include <stdbool.h>

float getTime()
{

   struct timespec tsp;

   clock_gettime(CLOCK_REALTIME, &tsp);   //Call clock_gettime to fill tsp

   return tsp.tv_nsec;
}

typedef struct Interpreters {
	char* tape;
	char* ptr;
	size_t tape_size;
} Interpreter;

char* AddCell(Interpreter *in) {
	char* tmp = (char*) realloc(in->tape, (++in->tape_size) * sizeof(char));

    if (tmp == NULL) {
        printf("Max memory reached!");
        exit(1);
    }

    if (!in->tape[in->tape_size-1]) {
        in->tape[in->tape_size-1] = 0;
    }

    return tmp;
}

char* RemoveCell(Interpreter *in) {
    char tail = in->tape[in->tape_size-1];
    if (tail == 0 && in->ptr != &in->tape[in->tape_size-1] && in->tape_size > 0) {
        char* tmp = (char*) realloc(in->tape, (--in->tape_size) * sizeof(char));
        if (tmp == NULL) {
            printf("Error garbage collecting somehow!");
            exit(1);
        }
        return tmp;
    }
    return in->tape;
}


void RunBF(Interpreter *in, char* code) {
	//The current character being ran.
	char current;
	//The nested loop count used for telling when the next none nested [] appears
	size_t nesc;
	for (size_t i = 0; code[i] != 0; i++) {
		current = code[i];
		switch (current) {
			case '>':
			//> Increases pointer position by one
			in->tape = AddCell(in);
			++in->ptr;
			break;
			case '<':
			//< Decreases pointer position by one
			--in->ptr;
			break;
			case '+':
			//+ Increases the value of the pointer by one
			++*in->ptr;
			break;
			case '-':
			//- Decreases the value of the pointer by one
			--*in->ptr;
			break;
			case '.':
			//. Prints the current byte to the std::cout
			putchar(*in->ptr);
			break;
			case ',':
			//, Takes a byte from the std::cin and sets it to the current byte
			*in->ptr = getchar();
			break;
			case '[':
			//[ if the current byte value is 0 we skip the instructions in the bracket and countinue after the matching ]
			if (*in->ptr == 0) {
				nesc = 1;
				while (nesc > 0) {
					current = code[--i];
					if (current == ']') {
						nesc--;
					} else if (current == '[') {
						nesc++;
					}
				}				
			}
			break;
			case ']':
			if (*in->ptr) {
			//] if the current byte is non zero we return to the last [
			nesc = 1;
            while (nesc > 0) {
                current = code[--i];
                if (current == '[') {
                    	nesc--;
                	} else if (current == ']') {
                    	nesc++;
                	}
            	}
			}
			break;
		}
		in->tape = RemoveCell(in);
	}
	free(in->tape);
}

int main(int argc, char const *argv[]) {
	if (argc > 1) {
		struct stat buf;
		if (stat(argv[1], &buf) != 0) {
			printf("File %s Was Not Found!", argv[1]);
			exit(1);
		}
		Interpreter i;
		i.tape_size = 1;
		i.tape = (char*) calloc(i.tape_size, sizeof(char));
		i.ptr = i.tape;
		FILE *fp;
		fp = fopen(argv[1], "r");
		char code[30000];
		fread(&code, sizeof(char), 30000, fp);
		fclose(fp);
		RunBF(&i, code);
		return 0;
	}
	printf("Please provide a file name to open!");
	exit(1);
}
