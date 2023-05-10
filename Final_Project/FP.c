#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <io.h>
#pragma warning(disable : 4996)

#define WILD "\x01"
#define BLUE "\x03"
#define GREEN "\x04"
#define RED "\x05"
#define YELLOW "\x06"
#define WILD4 "\x02"

//STRUCTS
typedef struct card_s {
	int player;
	int index;
	char color[7];
	int value;
	char action[15];
	struct card_s* pt;
} card;


//FUNCTION DECLARATION 
void shuffle(card inArr[], int len);
void dealHand(card inArr[], card** h, card** t, int playerNum);
void initializeDeck(card inpCard[]);
void printHand(card* h, int player);
void removeCard(card inArr[], card** h, card** t, int player, int cardNum);
void printCard(card inArr[], int cardNum);
void addCard(card inArr[], card** h, card** t, int player);
bool winnerCheck(card* h, int player);
void cardRules(int cardNum, int* player, card inArr[], card* h, int* skip, int* draw2, int* reverse, int* wild, int* wd4, int* match);
void reShuffleDeck(card inArr[]);


//MAIN FUNCTION
int main() {
	FILE* outp = fopen("uno.txt", "w");
	card deck[108], discard;
	card *head = NULL, *tail = NULL;
	int player = 1, numPlayer = 2, cardNum, playOrDraw = 0, skip = 0, reverse = 0, draw2 = 0, wild = 0, wd4 = 0, match = 0, color = 0;

	printf("Lets play UNO!\n\n");

	//Initialization---------------------------------------
	initializeDeck(deck);
	shuffle(deck, 108);
	fclose(outp);
	dealHand(deck, &head, &tail, 2);

	discard = deck[0];
	for (int i = 0; i < 108; i++)
		deck[i] = deck[i + 1];
	deck[107] = discard;

	while (deck[107].value == 14) {
		discard = deck[0];
		for (int i = 0; i < 108; i++)
			deck[i] = deck[i + 1];
		deck[107] = discard;
	}

	
	printf("Discard pile: ");
	printCard(deck, 107);
	printf("\n");
	


	//Hand play through-----------------------------------
	while (deck->index != 108) {
		//Check action conditions
		if (draw2 == 1) {
			addCard(deck, &head, &tail, player);
			addCard(deck, &head, &tail, player);
			draw2 = 0;
		}
		if (wd4 == 1) {
			addCard(deck, &head, &tail, player);
			addCard(deck, &head, &tail, player);
			addCard(deck, &head, &tail, player);
			addCard(deck, &head, &tail, player);
			wd4 = 0;
		}

		if (playOrDraw == 0) {
			printf("Player %d hand: ", player);
			printHand(head, player);
			printf("\nPlay a card (1) or Draw a card(2) \n");
			scanf("%d", &playOrDraw);
		}

		//Logic controlling the playing of a card
		if (playOrDraw == 1) {
			playOrDraw = 0;

			//Edit hand
			while (match == 0) {
				//Asks user to choose a card and then checks against rule set
				printf("Play card: \n");
				scanf("%d", &cardNum);
				cardRules(cardNum, &player, deck, head, &skip, &draw2, &reverse, &wild, &wd4, &match);
				
				//Checks if the user wants to draw a card instead
				if (match == 2){
					addCard(deck, &head, &tail, player);
					printf("\n");
				}
			}

			//Makes sure not to remove a card if user decieded to draw card instead
			if(match == 1)
				removeCard(deck, &head, &tail, player, cardNum);
			
			match = 0;

			//Test hand for victory condition
			if (winnerCheck(head, player) == true)
				break;

			//Change Wild suit
			if (strcmp(deck[107].action, "wd4") == 0 || strcmp(deck[107].action, "wild") == 0) {
				printf("Choose color: (1)\x03 (2)\x04 (3)\x05 (4)\x06 \n");
				scanf("%d", &color);
				switch (color) {
				case 1:
					strcpy(deck[107].color, BLUE);
					break;
				case 2:
					strcpy(deck[107].color, GREEN);
					break;
				case 3:
					strcpy(deck[107].color, RED);
					break;
				case 4:
					strcpy(deck[107].color, YELLOW);
					break;
				default:
					break;
				}
			}

			//Discard Pile
			printf("\n");
			printf("Discard pile: ");
			printCard(deck, 107);
			printf("\n");

		}

		//Logic for drawing of a card
		else if (playOrDraw == 2) {
			playOrDraw = 0;
			addCard(deck, &head, &tail, player);
			printf("\n");

			//Discard Pile
			printf("Discard pile: ");
			printCard(deck, 107);
			printf("\n");

		}


		//Logic if either of the choice is not made appropriatly
		else {
			while (playOrDraw != 1 && playOrDraw != 2) {
				printf("Choice not valid\n\n");
				printf("Player %d hand: ", player);
				printHand(head, player);
				printf("\nPlay a card (1) or Draw a card(2) \n");
				scanf("%d", &playOrDraw);
			}
		}

		

		if (deck[0].index == 108)
			reShuffleDeck(deck);

		//Cycle through players
		if (reverse == 0) {
			if (skip == 1)
				player = player;
			else if (player >= numPlayer)
				player = 1;
			else
				player++;
			skip = 0;
		}
		else if (reverse == 1) {
			if (skip == 1)
				player = player;
			else if (player <= 1)
				player = numPlayer;
			else
				player--;
			skip = 0;
		}
		
	}

	printf("Congrats! Player %d wins!!!!\n", player);

	return 0;
}

//FUNCTION IMPLIMENTATION

void reShuffleDeck(card inArr[]) {
	card temp1[108], temp2[108];
	card temp;
	int i = 0;

	temp = inArr[107];

	for (i = 0; inArr[i].index != 108; i++)
		temp1[i] = inArr[i];
	while (i < 108) {
		temp2[i] = inArr[i];
		i++;
	}
	
	for (i = 0; temp2[i].index != 108; i++)
		inArr[i] = temp2[i];
	while (i < 108) {
		inArr[i] = temp1[i];
		i++;
	}

	inArr[107] = temp;
}


//Test card to play against the rule set
void cardRules(int cardNum, int *player, card inArr[], card* h, int *skip, int *draw2, int *reverse, int *wild, int *wd4, int *match) {
	int i = 0, correct = 0, playOrDraw = 0, color = 0;

	//Starts at the designated player's hand
	while (h->player != *player) {
		h = h->pt;
	}
	//Finds target card in dynamic memory
	while (i < cardNum - 1) {
		h = h->pt;
		i++;
	}

	//Checks to make sure all the conditions are correct to play the card
	if (strcmp(h->color, inArr[107].color) == 0 || h->value == inArr[107].value || strcmp(h->action, "wild") == 0 || strcmp(h->action, "wd4") == 0) {
		*match = 1;

		//Alters player count for skip
		if (strcmp(h->action, "skip") == 0) {
			*skip = 1;
		}
		//Sets draw 2 marker to 1
		if (strcmp(h->action, "draw2") == 0)
			*draw2 = 1;
		//Checks current direction then sets the reverse marker
		if (strcmp(h->action, "reverse") == 0)
			if (*reverse == 0)
				*reverse = 1;
			else
				*reverse = 0;
		//Sets wild draw 4 marker and assigns color
		if (strcmp(h->action, "wd4") == 0)
			*wd4 = 1;
	}
	//Checks if user would prefer to draw a card instead
	else {
		printf("Press 1 to pick another card or 2 to draw a card\n");
		scanf("%d", &playOrDraw);
		if (playOrDraw == 2)
			*match = 2;
	}
	
}

//Check for winning condition
bool winnerCheck(card* h, int player) {
	int count = 0;
	
	while (h->player != player) {
		if (h->pt == NULL)
			return true;
		h = h->pt;
	}
	return false;
}

//Add a card from the deck to a player's hand
void addCard(card inArr[], card** h, card** t, int player) {
	card** target = h, * temp = NULL, hold;
	int i = 0;

	//Initiailize values for temp
	temp = (card*)malloc(sizeof(card));
	strcpy(temp->action, inArr[0].action);
	strcpy(temp->color, inArr[0].color);
	temp->index = inArr[0].index;
	temp->player = player;
	temp->value = inArr[0].value;

	//Move target to the correct position
	while ((*target)->player != player)
		target = &(*target)->pt;

	//Insert temp into array
	if (target == *h) {
		temp->pt = target;
		*h = temp;
	}
	else {
		temp->pt = (*target)->pt;
		(*target)->pt = temp;
	}

	//Adjust the deck values
	while (inArr[i].index != 108){
		inArr[i] = inArr[i + 1];
		i++;
	}
	for (int j = 0; j < i; j++) {
		inArr[j] = inArr[j + 1];
	}
	inArr[i].index = 108;
}

//Print individual card from the deck array
void printCard(card inArr[], int cardNum) {

	//Print the specified card
	//Skip
	if (inArr[cardNum].value == 10)
		printf("%s%s\t", inArr[cardNum].action, inArr[cardNum].color);
	//Reverse
	else if (inArr[cardNum].value == 11)
		printf("%s%s\t", inArr[cardNum].action, inArr[cardNum].color);
	//Draw
	else if (inArr[cardNum].value == 12)
		printf("%s%s\t", inArr[cardNum].action, inArr[cardNum].color);
	else if (strcmp(inArr[cardNum].action, "wild") == 0)
		printf("%s%s\t", inArr[cardNum].action, inArr[cardNum].color);
	else if (strcmp(inArr[cardNum].action, "wd4") == 0)
		printf("%s%s\t", inArr[cardNum].action, inArr[cardNum].color);
	else
		printf("%d%s\t", inArr[cardNum].value, inArr[cardNum].color);
	printf("\n");
}

//Remove card from hand and place in discard
void removeCard(card inArr[], card** h, card** t, int player, int cardNum) {
	int i = 0;
	card* target = *h, **temp = h;

	//Starts at the designated player's hand
	while (target->player != player) {
		target = target->pt;
	}
	//Finds target card in dynamic memory removes, and adds to end of deck
	while (i < cardNum - 1) {
		target = target->pt;
		i++;
	}
	while (*temp != target) {
		temp = &(*temp)->pt;
	}
	if (target == *h)
		*h = target->pt;	
	if (target == *t)
		*t = *temp;
	inArr[107] = *target;
	*temp = target->pt;
	free(target);

}

//Print the contents of a hand
void printHand(card* h, int player) {
	int j = 1;
	//Traverse the list
	while (h != NULL) {
		//Print each value for each card
		if (h->player == player) {
			//Skip
			if (h->value == 10)
				printf("(%d)%s%s\t", j, h->action, h->color);
			//Reverse
			else if (h->value == 11)
				printf("(%d)%s%s\t", j, h->action, h->color);
			//Draw 2
			else if (h->value == 12)
				printf("(%d)%s%s\t", j, h->action, h->color);
			else if (strcmp(h->action, "wild") == 0)
				printf("(%d)%s\t", j, h->action);
			else if (strcmp(h->action, "wd4") == 0)
				printf("(%d)%s\t", j, h->action);
			else
				printf("(%d)%d%s\t", j, h->value, h->color);
			j++;
		}
		h = h->pt;
	}
}

//Create a fresh deck of cards
void initializeDeck(card inpCard[]) {

	//Sets an initial index for each card
	for (int i = 0; i < 108; i++)					
		inpCard[i].index = i;

	//Set all players to zero
	for (int i = 0; i < 108; i++)
		inpCard[i].player = 0;
	
	//Initialize values for each card
	for (int i = 0; i < 108; i++) {
		if (inpCard[i].index >= 0 && inpCard[i].index <= 13)
			inpCard[i].value = i;
		if (inpCard[i].index >= 14 && inpCard[i].index <= 27)
			inpCard[i].value = i - 14;
		if (inpCard[i].index >= 28 && inpCard[i].index <= 41)
			inpCard[i].value = i - 28;
		if (inpCard[i].index >= 42 && inpCard[i].index <= 55)
			inpCard[i].value = i - 42;
		if (inpCard[i].index >= 56 && inpCard[i].index <= 68)
			inpCard[i].value = i - 55;
		if (inpCard[i].index >= 69 && inpCard[i].index <= 81)
			inpCard[i].value = i - 68;
		if (inpCard[i].index >= 82 && inpCard[i].index <= 94)
			inpCard[i].value = i - 81;
		if (inpCard[i].index >= 95 && inpCard[i].index <= 107)
			inpCard[i].value = i - 94;
	}

	// Initialize Suites by assigning grougs to a color
	for (int i = 0; i < 108; i++) {
		if (inpCard[i].index >= 0 && inpCard[i].index < 13)
			strcpy(inpCard[i].color, RED);
		if (inpCard[i].index == 13)
			strcpy(inpCard[i].color, WILD);
		if (inpCard[i].index >= 14 && inpCard[i].index < 27)
			strcpy(inpCard[i].color, YELLOW);
		if (inpCard[i].index == 27)
			strcpy(inpCard[i].color, WILD);
		if (inpCard[i].index >= 28 && inpCard[i].index < 41)
			strcpy(inpCard[i].color, GREEN);
		if (inpCard[i].index == 41)
			strcpy(inpCard[i].color, WILD);
		if (inpCard[i].index >= 42 && inpCard[i].index < 55)
			strcpy(inpCard[i].color, BLUE);
		if (inpCard[i].index == 55)
			strcpy(inpCard[i].color, WILD);
		if (inpCard[i].index >= 56 && inpCard[i].index < 68)
			strcpy(inpCard[i].color, RED);
		if (inpCard[i].index == 68)
			strcpy(inpCard[i].color, WILD4);
		if (inpCard[i].index >= 69 && inpCard[i].index < 81)
			strcpy(inpCard[i].color, YELLOW);
		if (inpCard[i].index == 81)
			strcpy(inpCard[i].color, WILD4);
		if (inpCard[i].index >= 82 && inpCard[i].index < 94)
			strcpy(inpCard[i].color, GREEN);
		if (inpCard[i].index == 94)
			strcpy(inpCard[i].color, WILD4);
		if (inpCard[i].index >= 95 && inpCard[i].index < 107)
			strcpy(inpCard[i].color, BLUE);
		if (inpCard[i].index == 107)
			strcpy(inpCard[i].color, WILD4);
	}

	//Initialize action for skip
	for (int i = 0; i < 108; i++) {
		if (inpCard[i].index == 10 || inpCard[i].index == 24 || inpCard[i].index == 38 ||
			inpCard[i].index == 52 || inpCard[i].index == 65 || inpCard[i].index == 78 ||
			inpCard[i].index == 91 || inpCard[i].index == 104) {
			strcpy(inpCard[i].action, "skip");
		}
		else
			strcpy(inpCard[i].action, "null");
	}

	//Initialize action reverse
	for (int i = 0; i < 108; i++) {
		if (inpCard[i].index == 11 || inpCard[i].index == 25 || inpCard[i].index == 39 ||
			inpCard[i].index == 53 || inpCard[i].index == 66 || inpCard[i].index == 79 ||
			inpCard[i].index == 92 || inpCard[i].index == 105) {
			strcpy(inpCard[i].action, "reverse");
		}
		
	}

	//Initialize action draw 2
	for (int i = 0; i < 108; i++) {
		if (inpCard[i].index == 12 || inpCard[i].index == 26 || inpCard[i].index == 40 ||
			inpCard[i].index == 54 || inpCard[i].index == 67 || inpCard[i].index == 80 ||
			inpCard[i].index == 93 || inpCard[i].index == 106) {
			strcpy(inpCard[i].action, "draw2");
		}

	}

	//Initialize action wild cards
	for (int i = 0; i < 108; i++) {
		if (inpCard[i].index == 13 || inpCard[i].index == 27 || inpCard[i].index == 41 ||
			inpCard[i].index == 55) {
			strcpy(inpCard[i].action, "wild");
		}

	}

	//Initialize action wild card draw 4
	for (int i = 0; i < 108; i++) {
		if (inpCard[i].index == 68 || inpCard[i].index == 81 || inpCard[i].index == 94 ||
			inpCard[i].index == 107) {
			strcpy(inpCard[i].action, "wd4");
		}

	}
}

//Deal hands
void dealHand(card inArr[], card** h, card** t, int playerNum) {
	card* temp = NULL;

	//Distribute card from the deck to a players hand using dynamic memory and rearranges the deck
	for (int i = 0; i < playerNum * 7; i++) {

		//Setting values and memory to card i
		temp = (card*)malloc(sizeof(card));
		temp->index = inArr[i].index;
		temp->value = inArr[i].value;
		strcpy(temp->action, inArr[i].action);
		strcpy(temp->color, inArr[i].color);
		
		//Set player identity
		if (i / 7 == 0)
			temp->player = 1;
		else
			temp->player = 2;

		//Setting the new pointers
		if (*h == NULL)
			*h = temp;
		else
			(*t)->pt = temp;
		temp->pt = NULL;
		*t = temp;


		//Remove the card from the deck
		for (int j = 0; j < 108; j++) {
			inArr[j] = inArr[j + 1];
		}
		//Set the last index to 108 for logic purposes
		inArr[107].index = 108;
	}
}

//Deck Shuffle function
void shuffle(card inArr[], int len) {

	srand(time(NULL));

	for (int i = 0; i < len; i++) {
		int swap = rand() % len;
		card temp = inArr[i];
		inArr[i] = inArr[swap];
		inArr[swap] = temp;
	}
}