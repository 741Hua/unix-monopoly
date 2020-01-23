#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define clear() printf("\033[H\033[J")

int main(int argc, char *argv[]) {
/*****************************************Safety Checks*************************************************/	
	int i, j, k; //loop counters
	if(argc != 5) {
		printf("\nIncorrect number of arguments provided.\nUsage: a.out path_length path_file property_file chance_file");
		exit(0);
	}
	for(i = 0; i < argc; i++) {
		if(argv[i] == NULL) {
			printf("\n%s could not be opened properly", argv[i]);
			exit(0);
		}
	}
/**************************************************Variable Declaration******************************************************************/
	int path_l = atoi(argv[1]);
	FILE *path_file = fopen(argv[2], "r"); 
	FILE *property_file = fopen(argv[3], "r");
	FILE *chance_file = fopen(argv[4], "r");
	const int START_BALANCE = 1500;
	const int BOARD_SIZE = 21;
	char board[BOARD_SIZE][BOARD_SIZE];
	int p_balance[] = {START_BALANCE, START_BALANCE}; //index 0 is p1 balance, index 1 is p2 balance
	int x1, y1, x2, y2, spaces, chanceY, chanceX, propY, propX, p1, p2, p3, r1, r2, r3; // variables for collecting file data
	int player_pos[] = {0, 0}; //positions of players, index 0 is p1, 1 is p2, numbers correspond to path index
	int coord[path_l][2]; //step by step coordinates of the path
	int properties[path_l][8]; //Property information only stored in row indices with same index as property coords. Column Structure: owner, property lvl, upgrade cost 1, 2, 3, rental cost 1, 2, 3
	int chance_pos[path_l]; //chance information only stored in row indeces with same indices as chance coords
	int prop_lvl;
	int turn = 1;
	int turn_lost = 0; //lost turn boolean
	int die1, die2; //vars set to random ints
	int jailed[] = {0, 0}; //bool array, 1 means jailed, index 0 is player 1, index 2 is player 2
	int jail_pos; // jail index position
	char user_input;
	char user_input_str[200];
	srand(time(0));

	//initialize arrays with zeros
	for(i=0; i < path_l; i++) {
		coord[i][0] = 0;
		coord[i][1] = 0;
		chance_pos[i] = 0;
		for(j = 0; j < 8; j++) {
			properties[i][j] = 3; //3 is used as dummy number
		}
	}
/********************************************Import path file*******************************************************/
	i = 0;
	while(fscanf(path_file, "(%d %d) -> (%d %d)\n", &y1, &x1, &y2, &x2) != EOF) {
		while(x1 != x2) {
			coord[i][1] = x1; coord[i][0] = y1;
			if(x1 < x2) x1++;
			else x1--;
			i++;
		}
		while(y1 != y2) {
			coord[i][0] = y1; coord[i][1] = x1;
			if(y1 < y2) y1++;
			else y1--;
			i++;
		}
	}
/************************************************Import property file****************************************************/
	while(fscanf(property_file, "(%d %d) %d %d %d | %d %d %d\n", &propY, &propX, &p1, &p2, &p3, &r1, &r2, &r3) != EOF) {
		for(j = 0; !(coord[j][0] == propY && coord[j][1] == propX) && j < path_l; j++);
		properties[j][0] = 0; properties[j][1] = 1; properties[j][2] = p1; properties[j][3] = p2; properties[j][4] = p3; properties[j][5] = r1; properties[j][6] = r2; properties[j][7] = r3;
	}
/*************************************************Import chance file********************************************************/
	i = 0;
	while(fscanf(chance_file, "(%d %d)\n", &chanceY, &chanceX) != EOF) {
		for(j = 0; !(coord[j][0] == chanceY && coord[j][1] == chanceX) && j < path_l; j++);
		if(i == 0) jail_pos = j; //JAIL
		else chance_pos[j] = 1; //1 means chance is here
		i++;
	}
/*******************************************************Board Creation*******************************************************/
	for(i = 0; i < BOARD_SIZE; i++) {
		for(j = 0; j < BOARD_SIZE; j++) {
			board[i][j] = ' '; //fill all other spaces with spaces
		}
		if(i==0) board[0][0] = ' ';
		else {
			board[0][i] = (i-1)%10 + 48; //fill in numbered borders
			board[i][0] = (i-1)%10 + 48;
		}
	}
	//Fill in arrows
	for(i = 0; i < path_l; i++) {
		if(coord[(i+1)%path_l][0] > coord[i][0]) board[coord[i][0] + 1][coord[i][1]+1] = 'v';
		else if(coord[(i+1)%path_l][0] < coord[i][0]) board[coord[i][0]+1][coord[i][1]+1] = '^';
		else if(coord[(i+1)%path_l][1] > coord[i][1]) board[coord[i][0]+1][coord[i][1]+1] = '>';
		else if(coord[(i+1)%path_l][1] < coord[i][1]) board[coord[i][0]+1][coord[i][1]+1] = '<';
	}
/******************************************Run Game******************************************************/
	while(p_balance[0] > 0 && p_balance[1] > 0) {	
		printf("\nPlayer %d's turn\nBalance: \033[1;32m$%d\033[0;37m", turn, p_balance[turn-1]);
		//print game board
		for(i = 0; i < BOARD_SIZE; i++) {
			printf("\n");
			for(j = 0; j < BOARD_SIZE; j++) {
				if(i-1 == coord[player_pos[0]][0] && j-1 == coord[player_pos[0]][1] && i-1 == coord[player_pos[1]][0] && j-1 == coord[player_pos[1]][1])
					printf("%-c\033[1;35m%-2c\033[0;37m", board[i][j], 'E');
				else if(i-1 == coord[player_pos[0]][0] && j-1 == coord[player_pos[0]][1])
					printf("%-c\033[1;36m%-2c\033[0;37m", board[i][j], 'A');
				else if(i-1 == coord[player_pos[1]][0] && j-1 == coord[player_pos[1]][1])
					printf("%-c\033[1;33m%-2c\033[0;37m", board[i][j], 'B');
				else printf("%-3c", board[i][j]);
			}
		}
		//roll
		if(turn == 1) {
			user_input = NULL;
			while(user_input != '\n') {
				printf("\nPress Enter to roll");
				scanf("%c", &user_input);
			}
		}
		die1 = rand()%6 + 1;
		die2 = rand()%6 + 1;
		printf("\nRoll: %d + %d", die1, die2);
		//doubles for jail
		if(jailed[turn-1] == 1 && die1 == die2) {
			printf("\nYou rolled doubles! Get out of JAIL");
			turn = turn%2+1;
			continue;
		}
		//collect GO money
		if(player_pos[turn-1] + die1 + die2 >= path_l) {
			p_balance[turn-1] += 200;
			printf("\n+$200 for passing GO, balance: \033[1;32m$%d\033[0;37m", p_balance[turn-1]);
		}
		player_pos[turn-1] = (player_pos[turn-1] + die1 + die2) % path_l;
		printf("\nYou landed on: (%d %d)", coord[player_pos[turn-1]][0], coord[player_pos[turn-1]][1]);
		prop_lvl = properties[player_pos[turn-1]][1]; // property lvl for prop at player pos, used as the index for many arrays later on, so stored in variable for readability
		//check if landed on property or chance
		user_input_str[0] = 'z';
		if(properties[player_pos[turn-1]][0] == 0){ //unowned property
			printf("\nSpace\tOwner\tUpgrade Stage\tUpgrade Cost\tRental Cost\n");
			printf("(%d %d)\t%s\t%d\t\t$%d\t\t$%d", coord[player_pos[turn-1]][0], coord[player_pos[turn-1]][1], "None", properties[player_pos[turn-1]][1], properties[player_pos[turn-1]][prop_lvl + 1], properties[player_pos[turn-1]][prop_lvl + 4]);
			if(p_balance[turn-1] - properties[player_pos[turn-1]][prop_lvl + 1] > 0) { //only allowed to purchase if enough funds
				while(user_input_str[0] != 'y' && user_input_str[0] != 'n' && user_input_str[0] != 'Y' && user_input_str[0] != 'N') {
					printf("\nPurchase property? ");
					if(turn == 2) {
						user_input_str[0] = 'y';
						printf("Yes");
					}
					else {
						scanf("%s", user_input_str);
						getchar();
					}
				}
			} else printf("\nCannot purchase property: Not enough funds");
			if(user_input_str[0] == 'Y' || user_input_str[0] == 'y') { //if had enough funds and chose to purchase, process the order
				properties[player_pos[turn-1]][0] = turn;
				p_balance[turn-1] -= properties[player_pos[turn-1]][prop_lvl + 1];
				printf("\nProperty purchased!\nBalance: \033[1;32m$%d\033[0;37m", p_balance[turn-1]);
			}
		} else if(properties[player_pos[turn-1]][0] == turn && prop_lvl < 3) { //upgrade property
			printf("\nSpace\tOwner\tUpgrade Stage\tUpgrade Cost\tRental Cost\n");
			printf("(%d %d)\tP%d\t%d\t\t$%d\t\t$%d", coord[player_pos[turn-1]][0], coord[player_pos[turn-1]][1], turn, properties[player_pos[turn-1]][1], properties[player_pos[turn-1]][prop_lvl + 1], properties[player_pos[turn-1]][prop_lvl + 4]);
			if(p_balance[turn-1] - properties[player_pos[turn-1]][prop_lvl + 1] > 0) {
				while(user_input_str[0] != 'y' && user_input_str[0] != 'n' && user_input_str[0] != 'Y' && user_input_str[0] != 'N') {
					printf("\nUpgrade property? ");
					if(turn == 2) {
						user_input_str[0] = 'y';
						printf("Yes");
					}
					else scanf("%s", user_input_str);
				}
			} else printf("\nCannot upgrade property: Not enough funds");
			if((user_input_str[0] == 'Y' || user_input_str[0] == 'y') && p_balance[turn-1] - properties[player_pos[turn-1]][prop_lvl + 1] > 0) {
				p_balance[turn-1] -= properties[player_pos[turn-1]][prop_lvl + 1];
				properties[player_pos[turn-1]][1]++;
				prop_lvl = properties[player_pos[turn-1]][1];
				printf("\nProperty upgraded!\nBalance: \033[1;32m$%d\033[0;37m", p_balance[turn-1]);
			}
		} else if(properties[player_pos[turn-1]][0] == turn%2+1) { //Land on other player's property
			printf("\nSpace\tOwner\tUpgrade Stage\tUpgrade Cost\tRental Cost\n");
			printf("(%d %d)\tP%d\t%d\t\t$%d\t\t$%d", coord[player_pos[turn-1]][0], coord[player_pos[turn-1]][1], turn%2+1, properties[player_pos[turn-1]][1], properties[player_pos[turn-1]][prop_lvl + 1], properties[player_pos[turn-1]][prop_lvl + 4]);
			p_balance[turn-1] -= properties[player_pos[turn-1]][prop_lvl + 4];
			printf("\nRent payed: $%d", properties[player_pos[turn-1]][prop_lvl + 4]);
			printf("\nBalance: \033[1;32m$%d\033[0;37m", p_balance[turn-1]);
			if(p_balance[turn-1] < 0) {
				printf("\nYou went bankrupt!");
				continue; // if bankrupt 
			}
		} else if(chance_pos[player_pos[turn-1]] == 1) { //land on chance space
			user_input = 'z';
			while(user_input != '\n') {
				printf("\nPress ENTER to draw a chance card: ");
				if(turn == 2) {
					user_input = '\n';
				}
				else scanf("%c", &user_input);
			}
			switch(rand()%6) {
				case 0: printf("\nYour rich uncle passed away, inherit \033[1;32m$100\033[0;37m"); 
					p_balance[turn-1] += 100;
					printf("\nBalance: \033[1;32m$%d\033[0;37m", p_balance[turn-1]);
					break;
				case 1: printf("\nGang Qu makes you buy lab manual, pay \033[1;31m$100\033[0;37m"); 
					p_balance[turn-1] -= 100;
					printf("\nBalance: \033[1;32m$%d\033[0;37m", p_balance[turn-1]);
					break;
				case 2: printf("\nThe dice love you, get an extra turn"); 
					turn = turn%2+1; //set to other players turn at end of loop their turn is skipped
					break;
				case 3: printf("\nLose a turn");
					turn_lost = turn;
					break;
				case 4: spaces = rand()%3+1; 
					printf("\nGang Qu makes you move forward %d spaces", spaces);
					player_pos[turn-1] = (player_pos[turn-1] + spaces) % path_l;
					printf("\nYou landed on: (%d %d)", coord[player_pos[turn-1]][0], coord[player_pos[turn-1]][1]);
					break;
				case 5: spaces = rand()%3+1; 
					printf("You disrepect Gang Qu, go back %d spaces", spaces); 
					player_pos[turn-1] = (player_pos[turn-1] + path_l - spaces) % path_l;
					printf("\nYou landed on: (%d %d)", coord[player_pos[turn-1]][0], coord[player_pos[turn-1]][1]);
					break;
				case 6: printf("\nYou got caught skipping Gang Qu's lecture, go to JAIL, do not pass GO, do not collect $200");
					player_pos[turn-1] = jail_pos;
					jailed[turn-1] = 1;
					break;
			}
		}
		user_input = 'z';
		while(user_input != '\n') {
			printf("\nPress Enter to end turn");
			scanf("%c", &user_input);
		}
		clear();
		if(turn_lost == turn%2+1) { //if both lose turn, then last player to lose turn is the only one who truly loses a turn
			turn = turn%2+1;
			turn_lost = 0;
		}
		turn = turn%2+1;
	}
	printf("\nPlayer %d wins!", turn%2+1);
	return 0;
}