﻿#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <signal.h>
#include <ctype.h> 



typedef struct {
	char name[30];
	char address[16];
	int daysInNum[7];
	char days[80];
}applicants;

int MAX_SIZE = 100;

void sendToOffice(applicants* applicantList, int size);
void addApplicant(applicants* applicantList, int size);
void modifyApplicant(applicants* applicantList, int size);
int deleteApplicant(applicants* applicantList, int size);
void getApplicantsByPlace(applicants* applicantList, int size);
void writeToFile(applicants* applicantList, int size, char* fileName);
int getFileSize(char* filename);
int dayToNum(char* days, int size);
void loadMenu(applicants* applicantList, int size);
bool contDays(char* nums, int day);
char* convertNumberIntoArray(unsigned int number);
void loadData(char* fileName, applicants* applicantList);
void removeStringTrailingNewline(char* str);
char* numsToStr(char* nums);
char* numToDay(int idx);
void strToNum(char* str, int* nums);
void printMenu();
void printDays();
void printFreeDays();
void handler(int signumber, siginfo_t* info, void* nonused);

char fileName[] = "applicantsdata.txt";
int limit[7] = { 10, 8, 5, 4, 5, 6, 7 };
int needed[7] = { 9, 7, 4, 4, 4, 5, 4 };
int workers[7] = { 0,0,0,0,0,0,0 };
int munkaNum = 0;
int main() {
	//Torlesnel ellenorzi, hogy volt-e adat, amit toroltunk. Csak akkor csokkenti a size-ot és valtoztatja a filet
	applicants* applicantList = (applicants*)malloc(sizeof(applicants) * MAX_SIZE);
	//Betolti az adatokat a strukturaba

	loadData(fileName, applicantList);

	printf("\n\n\n\n----------------Udvozlunk a Jo bor, jo egeszseg szoleszetben!----------------");
	loadMenu(applicantList, getFileSize(fileName));


	return 0;
}

void loadMenu(applicants* applicantList, int size) {
	char s = 0;
	printf("\n\nA rendszerben jelenleg %d jelentkezot regisztraltak\n", getFileSize(fileName));
	printMenu();
	while (s != 6) {

		scanf("\n%c", &s);
		switch (s) {
		case '1':
			addApplicant(applicantList, size);
			size++;
			writeToFile(applicantList, size, fileName);
			sleep(2);
			printMenu();
			break;
		case '2':
			modifyApplicant(applicantList, size);
			writeToFile(applicantList, size, fileName);
			sleep(2);
			printMenu();
			break;
		case '3':
			if (deleteApplicant(applicantList, size)) {
				size--;
				writeToFile(applicantList, size, fileName);
			}
			sleep(2);
			printMenu();
			break;
		case '4':
			getApplicantsByPlace(applicantList, size);
			sleep(2);
			printMenu();
			break;
		case '5':
			sendToOffice(applicantList, size);
			sleep(3);
			printMenu();
			break;
		case '6':
			printf("\nViszont latasra!\n");
			exit(1);
		default:
			printf("Rossz szamot adott meg! Probalja ujra!\n");
		}
	}
}

void handler(int signumber, siginfo_t* info, void* nonused) {
	munkaNum = info->si_value.sival_int;
}

void sendToOffice(applicants* applicantList, int size) {
	//Jenő telek, Lovas dűlő, Hosszú, Selyem telek, Malom telek és Szula
	//metszés, rügyfakasztó permetezés, tavaszi nyitás, horolás
	char area[5][30] = { "Jeno telek", "Lovas dulo", "Selyem telek", "Malom telek", "Szula" };
	char operation[4][30] = { "metszes", "rugyfakaszto permetezes", "tavaszi nyitas", "horolas" };

	signal(SIGTERM, handler);

	int pipefd[2]; // unnamed pipe file descriptor array
	int pipefd2[2]; // unnamed pipe file descriptor array

	int status;
	if (pipe(pipefd) < 0 || pipe(pipefd2)) { perror("Hiba a pipe nyitaskor!"); exit(EXIT_FAILURE); }

	pid_t child = fork();	// creating parent-child processes
	if (child < 0) { perror("Fork hiba"); exit(EXIT_FAILURE); }
	if (child > 0) {
		printf("\nSzulo1 indul!\n");
		sleep(1);
		close(pipefd[1]);  //Usually we close the unused write end

		char olvas[5][2][30];
		char munka[2][30];
		int len = 0;

		printf("\n\nGAZDATISZT OLVASTA AZ UZENETET:\n");
		int i = 0;
		while (i < 5) {
			char sz[30];
			char sz2[30];

			read(pipefd[0], &len, sizeof(int));
			read(pipefd[0], olvas[i][0], len); // reading max 100 chars

			read(pipefd[0], &len, sizeof(int));
			read(pipefd[0], olvas[i][1], len); // reading max 100 chars
			printf("%s : %s\n", olvas[i][0], olvas[i][1]);
			++i;
		}

		read(pipefd[0], &len, sizeof(int));
		read(pipefd[0], munka[0], len); // reading max 100 chars
		read(pipefd[0], &len, sizeof(int));
		read(pipefd[0], munka[1], len); // reading max 100 chars

		printf("\nMai melo:   %s : %s\n", munka[0], munka[1]);
		close(pipefd[0]); // Closing write descriptor 
		//printf("\nSzulo beirta az adatokat a csobe! \n");
		fflush(NULL); 	// flushes all write buffers (not necessary)
		sleep(1);
		kill(child, SIGKILL);

		//waitpid(child, &status, 0);
		//wait(&status);
		struct sigaction sigact;

		sigact.sa_sigaction = handler; //instead of sa_handler, we use the 3 parameter version
		sigemptyset(&sigact.sa_mask);
		sigact.sa_flags = SA_SIGINFO; //we need to set the siginfo flag 
		sigaction(SIGTERM, &sigact, NULL);

		pid_t child2 = fork();
		if (child2 < 0) { perror("The fork2 calling was not succesful\n"); exit(1); }
		if (child2 > 0) //the parent process, vezetés
		{
			waitpid(child, &status, 0);

			printf("\nSzulo2 indul! \n");
			close(pipefd2[0]);

			srand(time(0));
			int ranDay = rand() % 7 + 1;

			int i;
			int len = 0;
			int num = 0;
			for (i = 0; i < size; ++i) {
				int j;
				for (j = 0; j < 7; ++j) {
					if (applicantList[i].daysInNum[j] == ranDay) {
						num++;
					}
				}
			}
			write(pipefd2[1], &ranDay, sizeof(int));
			write(pipefd2[1], &num, sizeof(int));
			num = 0;
			for (i = 0; i < size; ++i) {
				int j;
				for (j = 0; j < 7; ++j) {
					if (applicantList[i].daysInNum[j] == ranDay) {
						printf("%d. jelentkezo: %s\n", num + 1, applicantList[i].name);
						len = strlen(applicantList[i].name) + 1;
						write(pipefd2[1], &len, sizeof(int));
						write(pipefd2[1], applicantList[i].name, len);
						num++;
					}
				}
			}



			len = strlen(munka[0]) + 1;
			write(pipefd2[1], &len, sizeof(int));
			write(pipefd2[1], munka[0], len);

			len = strlen(munka[1]) + 1;
			write(pipefd2[1], &len, sizeof(int));
			write(pipefd2[1], munka[1], len);

			//printf("\nHELY KESZ %s, %s\n", munka[0], munka[1]);
			//waitpid(child2, &status, 0);
			fflush(NULL);
			close(pipefd2[1]);
			sleep(1);

			pause();
			if (munkaNum != 0) {
				printf("\n\n\n\nVEZETOTOL MEGJOTT A NAPI ADAT: %i MUNKAST SZALLITOTT KI\n", munkaNum);
			}
			else { printf("\nHiba tortenhetett!\n"); }
			sleep(1);
			kill(child2, SIGKILL);
			//printf("\nSzulo2 befejezte!\n");
			//waitpid(child, &status, 0);

			printf("\nSzulo2 befejezte!\n");
		}
		else //child2 process, vezető munkásjárat
		{
			printf("\nGyerek2 indul!\n");
			close(pipefd2[1]);
			sleep(1);

			char melo[2][30];

			int s = 0;
			int day = 0;
			int i = 0;
			read(pipefd2[0], &day, sizeof(int));
			read(pipefd2[0], &s, sizeof(int));
			//printf("\nMEGKAPTAM %i\n", s);
			if (s == 0) {
				printf("\n\nA MAI NAPRA NEM JOTT SENKI DOLGOZNI:\n");
			}

			else { printf("\n\n%s NAPON JARATVEZETO OLVASTA AZ UTASLISTAT:\n", numToDay(day)); }

			len = 0;
			char app[s][30];
			while (i < s) {
				read(pipefd2[0], &len, sizeof(int));
				read(pipefd2[0], app[i], len);
				printf("%i. MUNKAS: %s\n", i + 1, app[i]);
				++i;
			}


			read(pipefd2[0], &len, sizeof(int));
			read(pipefd2[0], melo[0], len); // reading max 100 chars

			read(pipefd2[0], &len, sizeof(int));
			read(pipefd2[0], melo[1], len); // reading max 100 chars


			//printf("\nMai melo helye:   %s, ideje: %s\n", melo[0], melo[1]);
			close(pipefd2[0]);

			int needWork = needed[day] - workers[day];

			srand(time(0));
			i = 0;
			if (needWork > 0) {
				printf("\n\nMEG %i EMBER KELL A CSAPATBA!\n", needWork);
				while (i < needWork) {
					int ran = rand() % size;
					printf("\nA KISORSOLT SZERENCSES %i. MUNKAS:        %s", s + i + 1, applicantList[ran].name);
					++i;
				}
			}
			else {
				needWork = 0;
				printf("\n\nMEGVAN MINDENKI, MEHETUNK MELOZNI!");
			}
			printf("\n\nJARATVEZETO ELVITTE A %i MUNKAST %s-BA, HOGY VEGEZZEK EL A %s-t\n", s + needWork, melo[0], melo[1]);
			printf("\nJARATVEZETO KULDI A VEZETOSEGNEK A MUNKASOK SZAMAT: %i\n", s + needWork);
			union sigval s_value_int = { s + needWork };
			sigqueue(getppid(), SIGTERM, s_value_int);


			printf("\nGyerek2 befejezte!\n");
			pause();
		}
		//sleep(1);
		waitpid(child, &status, 0);
		waitpid(child2, &status, 0);
		printf("Szulo1 befejezte!\n");
	}

	else {
		printf("\nGyerek1 indul!\n");
		close(pipefd[0]);

		char pair[5][2][30];
		srand(time(0));

		int i;
		for (i = 0; i < 5; ++i) {

			int r = rand() % 4;

			strcpy(pair[i][0], area[i]);
			strcpy(pair[i][1], operation[r]);
			printf("\n%s : %s", pair[i][0], pair[i][1]);
		}
		printf("\n\n");


		int len = 0;
		i = 0;
		while (i < 5) {
			len = strlen(pair[i][0]) + 1;
			write(pipefd[1], &len, sizeof(int));
			write(pipefd[1], pair[i][0], len);
			//printf("Gyerek kuldi: %s\n", pair[i][0]);

			len = strlen(pair[i][1]) + 1;
			write(pipefd[1], &len, sizeof(int));
			write(pipefd[1], pair[i][1], len);
			//printf("Gyerek kuldi: %s\n", pair[i][1]);
			++i;
		}

		int r = rand() % 5;

		len = strlen(pair[r][0]) + 1;
		write(pipefd[1], &len, sizeof(int));
		write(pipefd[1], pair[r][0], len);

		len = strlen(pair[r][1]) + 1;
		write(pipefd[1], &len, sizeof(int));
		write(pipefd[1], pair[r][1], len);

		close(pipefd[1]);
		printf("\nGyerek1 befejezte!\n");
	}
	//waitpid(child, &status, 0);
}



void addApplicant(applicants* applicantList, int size) {
	if (size == MAX_SIZE) {
		MAX_SIZE += 100;
		applicantList = realloc(applicantList, sizeof(applicants) * MAX_SIZE);
		if (applicantList == NULL) {
			printf("HIBA! Nincs eleg memoria!\n");
			exit(1);
		}
	}

	applicantList[size].name[0] = 0;
	applicantList[size].address[0] = 0;
	applicantList[size].days[0] = 0;



	printf("\n\n\n----------Uj jelentkezo felvetele----------\n\n");

	printf("Kerlek add meg az uj jelentkezo nevet!\nNev: ");
	scanf("\n%[^\n]%*c", applicantList[size].name);
	removeStringTrailingNewline(applicantList[size].name);


	printf("\nKerlek add meg az jelentkező cimet!\nCim: ");
	scanf("\n%[^\n]%*c", applicantList[size].address);
	removeStringTrailingNewline(applicantList[size].address);

	printf("\nJelenleg ennyi emberrre van szuksegunk:\n\n");
	printFreeDays();

	printf("\n\nKerlek add meg azokat a napokat, amelyeken szeretnel dolgozni, szokozzel elvalasztva, ekezettel!\npl.:hétfő szerda vasárnap\n\nNapok: ");



	bool goodInput = false;
	while (!goodInput) {
		char days[100];
		fgets(days, sizeof(days), stdin);
		//printf("\n%s\n", days);

		strcpy(applicantList[size].days, days);
		// Extract the first token
		char* token = strtok(days, " ");
		// loop through the string to extract all other tokens
		int i = 0;
		int num[7] = { 9,9,9,9,9,9,9 };
		while (token != NULL) {
			int temp = 0;
			removeStringTrailingNewline(token);
			temp = dayToNum(token, sizeof(token) / sizeof(token[0]));
			if (contDays(num, temp)) {
				num[i] = 0;
			}
			else if (limit[temp - 1] - workers[temp - 1] < 1) {
				num[i] = 8;
			}
			else {
				num[i] = temp;
			}
			//printf("%s %d\n", token, num[i]); //printing each token

			++i;
			token = strtok(NULL, " ");
		}

		bool success = true;
		for (i = 0; i < 7; i++) {
			if (num[i] == 0) {
				success = false;
				printf("\n\nRossz napokat adott meg, probalja ujra!\nNapok: ");
			}
			if (num[i] == 8) {
				success = false;
				printf("\n\A megadott napok valamelyike betelt, adjon meg mas napokat!\nNapok: ");
			}
		}
		if (success) {
			goodInput = !goodInput;
			int j;
			for (j = 0; j < 7; j++) {
				applicantList[size].daysInNum[j] = num[j];
				if (num[j] < 8) {
					workers[num[j] - 1]++;
				}
				//printf("%d", applicantList[size].daysInNum[j]);
			}
			//memcpy(person.name, myname, strlen(myname) + 1);
		}
	}

	printf("\n\nFelvett szemely adatai:\n   Nev: %s\n   Cim: %s\n   Kivalasztott napok: %s\n", applicantList[size].name, applicantList[size].address, applicantList[size].days);
	printf("\nSikeres felvetel!\n");
	printf("Visszateres a menube..\n\n\n");

}

void modifyApplicant(applicants* applicantList, int size) {
	char name[30];
	int findApplicant = 0;
	printf("\n\n\n\n----------Meglevo jelentkezo modositasa----------\n\n\n");
	printf("Melyik meglevo jelentkezo adatait szeretned modositani? Add meg az jelentkezo nevet!\n");
	scanf("\n%[^\n]", name);
	removeStringTrailingNewline(name);

	int i;
	char answer = 0;
	char input[10];
	for (i = 0; i < size; ++i) {
		if (!strcmp(name, applicantList[i].name)) {
			findApplicant = 1;
			printf("Szeretne modositani az jelentkezo cimet?(i/n) ");
			scanf("\n%c", &answer);
			if (answer == 'i') {
				printf("\nKerlek add meg a jelentkezo cimet!\nCim: ");
				scanf("\n%[^\n]%*c", applicantList[i].address);
			}

			printf("Szeretne modositani a jelentkezes napjait?(i/n) ");
			scanf("\n%c", &answer);
			sleep(0.1);
			if (answer == 'i') {
				applicantList[size].days[0] = 0;
				printf("\nJelenleg ennyi emberrre van szuksegunk:\n\n");
				printFreeDays();

				printf("\n\nKerlek add meg azokat a napokat, amelyeken szeretnel dolgozni, szokozzel elvalasztva, ekezettel!\npl.:hétfő szerda vasárnap\n\nNapok: ");


				bool goodInput = false;
				while (!goodInput) {
					char days[100];
					fgets(days, sizeof(days), stdin);
					//printf("\n%s\n", days);

					strcpy(applicantList[i].days, days);

					// Extract the first token
					char* token = strtok(days, " ");
					// loop through the string to extract all other tokens
					int i = 0;
					int num[7] = { 9,9,9,9,9,9,9 };
					while (token != NULL) {
						int temp = 0;
						removeStringTrailingNewline(token);
						temp = dayToNum(token, sizeof(token) / sizeof(token[0]));
						if (contDays(num, temp)) {
							num[i] = 0;
						}
						else if (limit[temp - 1] - workers[temp - 1] < 1) {
							num[i] = 8;
						}
						else {
							num[i] = temp;
						}

						++i;
						token = strtok(NULL, " ");
					}

					bool success = true;
					for (i = 0; i < 7; i++) {
						if (num[i] == 0) {
							success = false;
							printf("\n\nRossz napokat adott meg, probalja ujra!\nNapok: ");
						}
						if (num[i] == 8) {
							success = false;
							printf("\n\A megadott napok valamelyike betelt, adjon meg mas napokat!\nNapok: ");
						}
					}
					if (success) {
						goodInput = !goodInput;
						int j;
						for (j = 0; j < 7; j++) {
							applicantList[i].daysInNum[j] = num[j];
							if (num[j] < 8) {
								workers[num[j] - 1]++;
							}
						}
					}
				}
			}
			break;
		}

	}
	if (findApplicant == 0) {
		printf("\nNincs ilyen nevu ugyfel! \n");
		printf("Modositas nem sikerult!! \n");
		printf("Visszateres a menube..\n\n\n");
	}
	else {
		printf("\nAz utas adatat sikeresen modositotta!\n");
		printf("Visszateres a menube..\n\n\n");
	}

}

int deleteApplicant(applicants* applicantList, int size) {
	char name[30];
	int i;
	int findApplicant = 0;
	printf("%d db ember", size);
	printf("\n\n\n----------Jelentkezo torlese a rendszerbol----------\n\n");
	printf("Melyik jelentkezo adatait szeretned torolni? Add meg a nevet!\n");
	scanf("\n%[^\n]", name);
	for (i = 0; i < size; ++i) {
		if (!strcmp(name, applicantList[i].name)) {
			int j;
			for (j = 0; j < 7; j++) {
				if (applicantList[i].daysInNum[j] < 8) {
					workers[applicantList[i].daysInNum[j] - 1]--;
				}
			}

			if (i != size - 1) {
				int j;
				for (j = i; j < size - 1; ++j) {
					applicantList[j] = applicantList[j + 1];
				}
				findApplicant = 1;
			}
			else {
				findApplicant = 1;
			}
			break;
		}
	}
	if (findApplicant == 0) {
		printf("Nincs ilyen nevu jelentkezo!\n");
		printf("Torles nem sikerult! Probalkozzon ujra!\n");
	}
	else {
		printf("\nTorles folyamatban, kerem varjon..\n");
	}
	return findApplicant;
}

int dayToNum(char command[], int size) {
	size_t i;

	for (i = 0; i < size; i++) {
		command[i] = tolower(command[i]);
	}
	if (strcmp(command, "hétfő") == 0) {
		return 1;
	}
	else if (strcmp(command, "kedd") == 0) {
		return 2;
	}
	else if (strcmp(command, "szerda") == 0) {
		return 3;
	}
	else if (strcmp(command, "csütörtök") == 0) {
		return 4;
	}
	else if (strcmp(command, "péntek") == 0) {
		return 5;
	}
	else if (strcmp(command, "szombat") == 0) {
		return 6;
	}
	else if (strcmp(command, "vasárnap") == 0) {
		return 7;
	}
	else {
		return 0;
	}

}

char* numToDay(int idx) {

	if (idx == 1) {
		return "hétfő";
	}
	else if (idx == 2) {
		return "kedd";
	}
	else if (idx == 3) {
		return "szerda";
	}
	else if (idx == 4) {
		return "csütörtök";
	}
	else if (idx == 5) {
		return "péntek";
	}
	else if (idx == 6) {
		return "szombat";
	}
	else if (idx == 7) {
		return "vasárnap";
	}
	else {
		return "";
	}

}

void removeStringTrailingNewline(char* str) {
	if (str == NULL)
		return;
	int length = strlen(str);
	if (str[length - 1] == '\n')
		str[length - 1] = '\0';
}

bool contDays(char* nums, int day) {
	int i;

	for (i = 0; i < 7; ++i) {
		if (nums[i] == day) {
			return true;
		}
	}
	return false;
}

void getApplicantsByPlace(applicants* applicantList, int size) {

	char c = 0;
	int i;
	int num = 0;
	int result;



	/*for (i = 0; i < size; ++i) {
		printf("Nev: %s", applicantList[i].name);
		printf("\n");
	}*/



	printf("\n\n\n----------Jelentkezok listazasa nap szerint----------\n\n");
	printf("Add meg melyik nap szerint szeretnel listazni!\n");
	printDays();
	printf("8.) Osszes nap listazasa\n");
	printf("\nValasztott hely: ");
	while (c != 1 || c != 2 || c != 3 || c != 4 || c != 5 || c != 6 || c != 7 || c != 8) {
		scanf("\n%c", &c);

		for (i = 0; i < size; ++i) {


			int j;
			for (j = 0; j < 7; ++j) {
				if (applicantList[i].daysInNum[j] == (c - '0')) {
					printf("%d. jelentkezo: %s\n", num + 1, applicantList[i].name);
					num++;
				}
				if ((c - '0') == 8) {
					printf("%d. jelentkezo: %s\n", num + 1, applicantList[i].name);
					num++;
					break;
				}
			}

		}


		printf("\nA valasztott napon %d db jelentkezo van regisztralva\n", num);
		printf("Visszateres a menube..\n");
		break;
	}
}

void writeToFile(applicants* applicantList, int size, char* fileName) {

	char applicantText[80];
	char str[8];
	remove(fileName);
	FILE* fl;
	fl = fopen(fileName, "w");

	//napok
	/*applicantText[0] = 0;
	strcat(applicantText, "10,2,3,4,5,6,7;");
	fprintf(fl, "%s\n", applicantText);*/

	int i;
	for (i = 0; i < size; ++i) {

		if (strlen(applicantList[i].name) != 0) {
			applicantText[0] = 0;
			str[0] = 0;

			strcat(applicantText, applicantList[i].name);
			strcat(applicantText, ";");
			strcat(applicantText, applicantList[i].address);
			strcat(applicantText, ";");
			sprintf(str, "%d%d%d%d%d%d%d\0", applicantList[i].daysInNum[0], applicantList[i].daysInNum[1], applicantList[i].daysInNum[2],
				applicantList[i].daysInNum[3], applicantList[i].daysInNum[4], applicantList[i].daysInNum[5], applicantList[i].daysInNum[6]);

			//str[7] = '\0';

			/*int j;
			for (j = 0; j < 7; ++j)
			{
				str[j] = applicantList[i].daysInNum[j] + '0';
			}
			printf("what:    %s\n", str);*/

			/*char str[30];
			char* out[7];
			int j;
			for (j = 0; j < 7; ++j)
			{
				snprintf(str, 30, "%d", applicantList[i].daysInNum[j]);
				out[j] = strdup(str);
			}*/


			//int j;
			//for (j = 0; j < 7; ++j)
			//{
			//	str[j] = '0' + applicantList[i].daysInNum[j];
			//	//sprintf(str[j], "%d", applicantList[i].daysInNum[j]);
			//	printf("%d", applicantList[i].daysInNum[j]);
			//	//strcat(applicantText, '0' + applicantList[i].daysInNum[j]);
			//}


			//printf("szamok:    %s\n", str);


			strcat(applicantText, str);
			strcat(applicantText, ";");

			//printf("%s\n", applicantText);
			fprintf(fl, "%s\n", applicantText);

		}
	}
	fclose(fl);
}

int getFileSize(char* fileName) {
	char line[80];
	char c;
	FILE* fl;
	fl = fopen(fileName, "r+");
	if (fl == NULL) {
		fl = fopen(fileName, "w");
		printf("Az adatbazis ures. Fájl letrehozasa...\n");
		if (fl == NULL) {
			fprintf(stderr, "Hiba a fajl letrehozasa kozben!\n");
			exit(1);
		}
		else {
			printf("A fajl letrehozasa megtortent.\n");
		}
	}
	int i = 0;
	while (fgets(line, sizeof(line), fl)) {
		++i;
		if (i == MAX_SIZE) {
			MAX_SIZE += 100;
		}
	}
	fclose(fl);
	return i;
}

void strToNum(char* str, int* nums) {
	int j;
	for (j = 0; j < 7; ++j) {
		nums[j] = str[j];
	}
}

void loadData(char* fileName, applicants* applicantList) {
	char line[80];
	FILE* fl;
	fl = fopen(fileName, "ab+");
	if (fl == NULL) {
		fprintf(stderr, "Hiba a fajl megnyitasakor! \n");
		exit(1);
	}
	int i = 0;
	while (fgets(line, sizeof(line), fl)) {
		char temp[12] = { 0 };
		sscanf(line, "%[^;];%[^;];%[^;]",
			applicantList[i].name, applicantList[i].address, temp);
		//strToNum(temp, applicantList[i].daysInNum);




		/*int j;
		for (j = 0; j < 7; ++j)
		{
			applicantList[i].daysInNum[j] = temp[j];
		}*/

		int z = atoi(temp);


		char arr[8]; //8 digits + \0

		if ((snprintf(arr, 8, "%d", z) == 7)) { //return the 8 characters that were printed
			int c;
			for (c = 0; c < 7; c++)
				applicantList[i].daysInNum[c] = arr[c] - '0';
		}


		int j = 0;
		while (j < 7 && applicantList[i].daysInNum[j] != 9) {

			workers[applicantList[i].daysInNum[j] - 1]++;
			++j;
		}


		/*	int c;
			for (c = 0; c < 7; c++)
			printf("%d::", applicantList[i].daysInNum[c]);*/


			//printf("betoltotte       %d::\n", z);
			//printf("jol tölti be: %s\n", line);
		++i;
	}

	fclose(fl);
}

void printMenu() {
	printf("\n\n\n\n--------------------MENU--------------------\n\n");
	printf("\nMilyen funkciot szeretnel hasznalni? Nyomd le a megfelelo szamot!\n\n");
	printf("1.)	Uj jelentkezo felvetele\n");
	printf("2.)	Jelentkezo modositasa\n");
	printf("3.)	Jelentkezes torlese\n");
	printf("4.)	Jelentkezok listazasa nap szerint\n");
	printf("5.)     Napi jelentes kuldese\n");
	printf("6.)     Kilepes\n\nValasztott funkcio: ");
}
void printDays() {
	printf("1.) Hétfő\n");
	printf("2.) Kedd\n");
	printf("3.) Szerda\n");
	printf("4.) Csütörtök\n");
	printf("5.) Péntek\n");
	printf("6.) Szombat\n");
	printf("7.) Vasárnap\n");
}
void printFreeDays() {
	printf("\nHétfő: %d szabad hely\n", limit[0] - workers[0]);
	printf("Kedd: %d szabad hely\n", limit[1] - workers[1]);
	printf("Szerda: %d szabad hely\n", limit[2] - workers[2]);
	printf("Csütörtök: %d szabad hely\n", limit[3] - workers[3]);
	printf("Péntek: %d szabad hely\n", limit[4] - workers[4]);
	printf("Szombat: %d szabad hely\n", limit[5] - workers[5]);
	printf("Vasárnap: %d szabad hely\n", limit[6] - workers[6]);
}


//void numsToStr(char* text, char* nums) {
//	char str[8];
//	sprintf(str, "%d%d%d%d%d%d%d", nums[0], nums[1], nums[2], nums[3], nums[4], nums[5], nums[6]);
//	
//	str[7] = '\0';
//	printf("szam: %s", str);
//	return &str;
//
//	char d[8];
//	strcat(d, numsToStr(applicantList[i].daysInNum));
//	printf("szamokkkkkk: %s\n", d);
//	strcat(applicantText, d);
//	strcat(applicantText, ";");
//}


//char* convertNumberIntoArray(unsigned int number) {
//	unsigned int length = (int)(log10((float)number)) + 1;
//	char* arr = (char*)malloc(length * sizeof(char)), * curr = arr;
//	do {
//		*curr++ = number % 10;
//		number /= 10;
//	} while (number != 0);
//	return arr;
//}