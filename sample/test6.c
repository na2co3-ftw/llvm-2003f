
int a;
int b = 10;
const int c = 30;
extern int d;

int test1() {
	return a + b + c + d;
}

int (*test2())() {
	return &test1;
}

int test3(int n) {
	switch(n) {
		case 0:	return 10;
		case 1:	return 20;
		case 2:	return 30;
		case 3:	return 40;
	}
	return 0;
}

char *test4() {
	return "test";
}
