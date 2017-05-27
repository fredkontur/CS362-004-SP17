/******************************************************************************
 * Filename:     unittest4.c
 * Author:       Fred Kontur
 * Date Written: April 23, 2017
 * Last Edited:  May 20, 2017
 * Description:  This file contains a unit test for the isGameOver() function 
 *               in dominion.c
 * Business Requirements for isGameOver():
 * 1. If stack of province cards is empty, then report that the game is over.
 * 2. If three of the supply piles are empty, then report that the game is
 *    over.
 * 3. If neither one of the two above conditions apply, then report that the
 *    game is not over.
******************************************************************************/
#include "dominion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_NUM_PLAYERS 4

// Returns 1 if decks are the same, 0 if different
int compDecks3(int* arr1, int* arr2, int deckSize) {
   int i;

   for(i = 0; i < deckSize; i++) {
      if(arr1[i] != arr2[i]) {
         return 0;
      }
   }

   return 1;
}

// This function compares 2 game states. It returns 1 if the states are the 
// same, 0 if different.
int compStates3(struct gameState s1, struct gameState s2) {
   int i, res = 1;

   if((s1.numPlayers) != (s2.numPlayers)) {
      printf("isGameOver() changed the number of players in the game.\n");
      res = 0;
   }
   for(i = 0; i <= treasure_map; i++) {
      if((s1.supplyCount[i]) != (s2.supplyCount[i])) {
         printf("isGameOver() changed the number of cards in"); 
         printf(" the supply deck.\n");
         res = 0;
      }
      if((s1.embargoTokens[i]) != (s2.embargoTokens[i])) {
         printf("isGameOver() changed the number of embargo tokens.\n");
         res = 0;
      }
   }
   if((s1.outpostPlayed) != (s2.outpostPlayed)) {
      printf("isGameOver() changed the count of outposts played.\n");
      res = 0;
   }
   if((s1.whoseTurn) != (s2.whoseTurn)) {
      printf("isGameOver() changed whose turn it is.\n");
      res = 0;
   }
   if((s1.phase) != (s2.phase)) {
      printf("isGameOver() changed the phase variable.\n");
      res = 0;
   }
   if((s1.numActions) != (s2.numActions)) {
      printf("isGameOver() changed the number of actions counter.\n");
      res = 0;
   }
   if((s1.numBuys) != (s2.numBuys)) {
      printf("isGameOver() changed the buys counter.\n");
      res = 0;
   }
   for(i = 0; i < s1.numPlayers; i++) {
      if((s1.deckCount[i]) != (s2.deckCount[i])) {
         printf("isGameOver() changed the number of cards in the deck of"); 
         printf("  Player %d.\n", i);
         res = 0;
      }
      if(!(compDecks3(s1.deck[i], s2.deck[i], s1.deckCount[i]))) {
         printf("isGameOver() changed the cards in the deck");
         printf(" of Player %d.\n", i);
         res = 0;
      }
      if((s1.handCount[i]) != (s2.handCount[i])) {
         printf("isGameOver() changed the number of cards in the hand");
         printf(" of Player %d.\n", i);
         res = 0;
      }
      if(!(compDecks3(s1.hand[i], s2.hand[i], s1.handCount[i]))) {
         printf("isGameOver() changed the cards in the hand");
         printf(" of Player %d.\n", i);
         res = 0;
      }
      if((s1.discardCount[i]) != (s2.discardCount[i])) {
         printf("isGameOver() changed the number of cards in the discard");
         printf(" pile for Player %d.\n", i);
         res = 0;
      }
      if(!(compDecks3(s1.discard[i], s2.discard[i], s1.discardCount[i]))) {
         printf("isGameOver() changed the cards in the discard");
         printf(" pile for Player %d.\n", i);
         res = 0;
      }
   }
   if((s1.playedCardCount) != (s2.playedCardCount)) {
      printf("isGameOver() changed the played card counter.\n");
      res = 0;
   }
   if(!(compDecks3(s1.playedCards, s2.playedCards, s1.playedCardCount))) {
      printf("isGameOver() changed the cards in the played card");
      printf(" pile.");
      res = 0;
   }
   return res;
}

void resetStates3(struct gameState *state1, struct gameState *state2, 
                 int numPlayer, int* k, int seed) {
   // First, clear the states
   memset(state1, 0, sizeof(struct gameState));
   memset(state2, 0, sizeof(struct gameState));

   // Now, initialize one of the states and then copy it
   initializeGame(numPlayer, k, seed, state1);
   memcpy(state2, state1, sizeof(struct gameState));
}

void testIsGameOver() {
   int numPlayer = TEST_NUM_PLAYERS, res;
   struct gameState state, copyState;
   int seed = 123456;

   // Create an array of kingdom cards
   int k[] = {adventurer, council_room, feast, gardens, mine,
              remodel, smithy, village, baron, great_hall};

   printf("\n***************Tests for isGameOver() function***************\n");

   // Test isGameOver() for a game that has just begun
   resetStates3(&state, &copyState, numPlayer, k, seed);
   res = isGameOver(&state);
   if(res) {
      printf("###Incorrect result - isGameOver() said newly started game was ");
      printf("over.\n");
   }
   else {
      if(compStates3(state, copyState)) {
         printf("Correct result for newly started game.\n");
      }
      else {
         printf("###Incorrect result for newly started game. ");
         printf("See description above.\n");
      }
   }
   // Test isGameOver() for a game with no province cards and all supply piles
   // still present
   resetStates3(&state, &copyState, numPlayer, k, seed);
   state.supplyCount[province] = 0;
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = isGameOver(&state);
   if(!res) {
      printf("###Incorrect result - isGameOver() said game with no province ");
      printf("cards was not over.\n");
   }
   else {
      if(compStates3(state, copyState)) {
         printf("Correct result for game with no province cards.\n");
      }
      else {
         printf("###Incorrect result for game with no province cards. ");
         printf("See description above.\n");
      }
   }
   // Test isGameOver() with 3 supply piles empty and province cards still
   // present
   resetStates3(&state, &copyState, numPlayer, k, seed);
   state.supplyCount[adventurer] = 0;
   state.supplyCount[smithy] = 0;
   state.supplyCount[baron] = 0;
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = isGameOver(&state);
   if(!res) {
      printf("###Incorrect result - isGameOver() said game with 3 empty ");
      printf("supply piles was not over.\n");
   }
   else {
      if(compStates3(state, copyState)) {
         printf("Correct result for game with 3 empty supply piles.\n");
      }
      else {
         printf("###Incorrect result for game with 3 empty supply piles. ");
         printf("See description above.\n");
      }
   }
   // Test isGameOver() with 2 supply piles empty and province cards still
   // present 
   resetStates3(&state, &copyState, numPlayer, k, seed);
   state.supplyCount[council_room] = 0;
   state.supplyCount[remodel] = 0;
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = isGameOver(&state);
   if(res) {
      printf("###Incorrect result - isGameOver() said game with 2 empty ");
      printf("supply piles was over.\n");
   }
   else {
      if(compStates3(state, copyState)) {
         printf("Correct result for game with 2 empty supply piles.\n");
      }
      else {
         printf("###Incorrect result for game with 2 empty supply piles. ");
         printf("See description above.\n");
      }
   }
}

int main(int argc, char *argv[]) {
   testIsGameOver();
   return 0;
}
