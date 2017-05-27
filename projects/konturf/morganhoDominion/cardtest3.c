/******************************************************************************
 * Filename:     cardtest3.c
 * Author:       Fred Kontur
 * Date Written: April 24, 2017
 * Last Edited:  May 20, 2017
 * Description:  This file contains a unit test for the Feast card in  
 *               dominion.c
 * Business Requirements for Feast:
 * 1. Feast card is trashed, and a card costing up to 5 is gained from the 
 *    supply, using up an Action.
 * 2. No other change to the gameState is made except that required to 
 *    accomplish the above action.
 * 3. A player is not permitted to use a Feast card to gain a card from the
 *    supply costing more than 5.
 * 4. A player is not permitted to use a Feast card to gain a card that is
 *    not in the supply.
******************************************************************************/
#include "dominion.h"
#include "dominion_helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_NUM_PLAYERS 2
#define TEST_PLAYER 1

// Returns 1 if decks are the same, 0 if different
int compDecks6(int* arr1, int* arr2, int deckSize) {
   int i;

   for(i = 0; i < deckSize; i++) {
      if(arr1[i] != arr2[i]) {
         return 0;
      }
   }

   return 1;
}

// This function compares 2 game states with the exception of the supply cards, 
// numActions, and the hand and discard pile of the current player. It returns 
// 1 if the states are the same, 0 if different.
int compStates6(struct gameState s1, struct gameState s2, int currPlayer) {
   int i, res = 1;

   if((s1.numPlayers) != (s2.numPlayers)) {
      printf("Feast changed the number of players in the game.\n");
      res = 0;
   }
   if((s1.outpostPlayed) != (s2.outpostPlayed)) {
      printf("Feast changed the count of outposts played.\n");
      res = 0;
   }
   if((s1.whoseTurn) != (s2.whoseTurn)) {
      printf("Feast changed whose turn it is.\n");
      res = 0;
   }
   if((s1.phase) != (s2.phase)) {
      printf("Feast changed the phase variable.\n");
      res = 0;
   }
   if((s1.numBuys) != (s2.numBuys)) {
      printf("Feast changed the buys counter.\n");
      res = 0;
   }
   for(i = 0; i < s1.numPlayers; i++) {
      if((s1.deckCount[i]) != (s2.deckCount[i])) {
         printf("Feast changed the number of cards in the deck of"); 
         printf("  Player %d.\n", i);
         res = 0;
      }
      if(!(compDecks6(s1.deck[i], s2.deck[i], s1.deckCount[i]))) {
         printf("Feast changed the cards in the deck");
         printf(" of Player %d.\n", i);
         res = 0;
      }
      if(i != currPlayer) {
         if((s1.handCount[i]) != (s2.handCount[i])) {
            printf("Feast changed the number of cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks6(s1.hand[i], s2.hand[i], s1.handCount[i]))) {
            printf("Feast changed the cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if((s1.discardCount[i]) != (s2.discardCount[i])) {
            printf("Feast changed the number of cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks6(s1.discard[i], s2.discard[i], s1.discardCount[i]))) {
            printf("Feast changed the cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
      }
   }
   if((s1.playedCardCount) != (s2.playedCardCount)) {
      printf("Feast changed the played card counter.\n");
      res = 0;
   }
   if(!(compDecks6(s1.playedCards, s2.playedCards, s1.playedCardCount))) {
      printf("Feast changed the cards in the played card");
      printf(" pile.");
      res = 0;
   }
   return res;
}

// Checks the changes in gameState that should have been caused by the Feast
// card. If the changes are valid, it returns 1. If not, it returns 0.
int checkFeastChanges(struct gameState originalState,
                      struct gameState finalState, int currPlayer)
{
   int i, chosenCard, n = 0, res = 1;
   int orgnlFstCnt = 0, fnlFstCnt = 0;
   int orgnlHndCnt = originalState.handCount[currPlayer];
   int fnlHndCnt = finalState.handCount[currPlayer];
   int orgnlDscrdCnt = originalState.discardCount[currPlayer];
   int fnlDscrdCnt = finalState.discardCount[currPlayer];

   // Make sure that a card was removed from the player's hand and the chosen
   // card was discarded
   if(orgnlHndCnt != (fnlHndCnt + 1)) {
      printf("Too many cards in hand after Feast is played\n");
      res = 0;
   }
   // Determine if Feast card was taken out of hand
   for(i = 0; i < orgnlHndCnt; i++) {
      if(originalState.hand[currPlayer][i] == feast) {
          orgnlFstCnt++;
       }
   }
   for(i = 0; i < fnlHndCnt; i++) {
      if(finalState.hand[currPlayer][i] == feast) {
          fnlFstCnt++;
       }
   }
   if(orgnlFstCnt != (fnlFstCnt + 1)) {
      printf("The Feast card was not removed from the hand\n");
      res = 0;
   }
   // Find the card that was chosen from the supply
   for(i = curse; i <= treasure_map; i++) {
      if(originalState.supplyCount[i] != finalState.supplyCount[i]) {
         chosenCard = i;
         if(finalState.supplyCount[i] > originalState.supplyCount[i]) {
            printf("Too many of Card #%d in supply after Feast is ", i);
            printf("played\n");
            res = 0;
            break;
         }
         else {
            n = originalState.supplyCount[i] - finalState.supplyCount[i];
         }
      }
   }
   // Make sure 1 card was taken from the supply, no more and no less
   if(n > 1) {
      printf("More than 1 card was taken from the supply\n");
      res = 0;
   }
   else if(n < 1) {
      printf("No card was taken from the supply\n");
      res = 0;
   }
   else {
      // Check that the card taken did not cost more than 5
      if(getCost(chosenCard) > 5) {
         printf("Card chosen from the supply cost more than 5\n");
         res = 0;
      }
   }
   // Check to make sure that the chosen card was placed in the discard pile
   // as the rules for Feast require
   if(fnlDscrdCnt != (orgnlDscrdCnt + 1)) {
      printf("Discard count is not correct after Feast is played\n");
      res = 0;
   }
   if(finalState.discard[currPlayer][orgnlDscrdCnt] != chosenCard) {
      printf("Chosen card was not placed in discard pile as required\n");
      res = 0;
   }
   return res;
}

void resetStates6(struct gameState *state1, struct gameState *state2, 
                 int numPlayer, int* k, int seed) {
   // First, clear the states
   memset(state1, 0, sizeof(struct gameState));
   memset(state2, 0, sizeof(struct gameState));

   // Now, initialize one of the states and then copy it
   initializeGame(numPlayer, k, seed, state1);
   memcpy(state2, state1, sizeof(struct gameState));
}

void testFeast() {
   int p = TEST_PLAYER, numPlayer = TEST_NUM_PLAYERS, res, res1, res2;
   struct gameState state, copyState;
   int seed = 123456;

   // Create an array of kingdom cards
   int k[] = {adventurer, council_room, feast, gardens, mine,
              remodel, smithy, village, baron, treasure_map};

   // Create dummy hands
   int handWithTreas[] = {copper, copper, copper, estate, feast};
   int handNoTreas[] = {adventurer, smithy, mine, feast, duchy}; 

   printf("\n***************Tests for Feast Card***************\n");
   printf("*Test Conditions: #Players = %d, ", numPlayer);
   printf("Current Player = %d*\n", p);

   // Test Feast with a hand with no Treasures and gaining a card that costs 5
   resetStates6(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], handNoTreas, sizeof(handNoTreas));
   state.handCount[p] = (sizeof(handNoTreas) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(3, duchy, 0, 0, &state);
   if(res != 0) {
      printf("###Incorrect result playing Feast in hand with no Treasures ");
      printf("to gain a cost 5 card.\n");
   }
   else {
      res1 = compStates6(copyState, state, p);
      res2 = checkFeastChanges(copyState, state, p);
      if((res1 == 0) || (res2 == 0)) {
         printf("###Incorrect result playing Feast in hand with no ");
         printf("Treasures to gain a cost 5 card. See description above.\n");
      }
      else {
         printf("Correct result playing Feast in hand with no Treasures to ");
         printf("gain a cost 5 card.\n");
      }
   }
   // Test Feast with a hand with no Treasures and gaining a card that costs 4
   resetStates6(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], handNoTreas, sizeof(handNoTreas));
   state.handCount[p] = (sizeof(handNoTreas) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(3, treasure_map, 0, 0, &state);
   if(res != 0) {
      printf("###Incorrect result playing Feast in hand with no Treasures ");
      printf("to gain a cost 4 card.\n");
   }
   else {
      res1 = compStates6(copyState, state, p);
      res2 = checkFeastChanges(copyState, state, p);
      if((res1 == 0) || (res2 == 0)) {
         printf("###Incorrect result playing Feast in hand with no ");
         printf("Treasures to gain a cost 4 card. See description above.\n");
      }
      else {
         printf("Correct result playing Feast in hand with no Treasures to ");
         printf("gain a cost 4 card.\n");
      }
   }
   // Test Feast with a hand with no Treasures to try to gain a card that is
   // no longer left in supply
   // This test will be skipped as it causes an infinite loop
   printf("###Incorrect result playing Feast to try to gain a card ");
   printf("that is no longer in the supply causes infinite loop.\n");
   resetStates6(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], handNoTreas, sizeof(handNoTreas));
   state.handCount[p] = (sizeof(handNoTreas) / sizeof(int));
   state.supplyCount[duchy] = 0;
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(3, duchy, 0, 0, &state);
   if(res != -1) {
      printf("###Incorrect result playing Feast to try to gain a card ");
      printf("that is no longer in the supply.\n");
   }
   else {
      printf("Correct result playing Feast to unsuccessfully gain a card ");
      printf("that is no longer in the supply.\n");
   }
   // Test Feast with a hand with no Treasures and gaining a card that costs 
   // more than 5
   resetStates6(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], handNoTreas, sizeof(handNoTreas));
   state.handCount[p] = (sizeof(handNoTreas) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(3, gold, 0, 0, &state);
   if(res != -1) {
      printf("###Incorrect result playing Feast to try to gain a cost ");
      printf("6 card.\n");
   }
   else {
      printf("Correct result playing Feast to unsuccessfully try to ");
      printf("gain a cost 6 card.\n");
   }
   // Test Feast with a hand with 3 coppers to try to gain a card that costs
   // more than 5
   resetStates6(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], handWithTreas, sizeof(handWithTreas));
   state.handCount[p] = (sizeof(handWithTreas) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(4, adventurer, 0, 0, &state);
   if(res != 0) {
      printf("###Incorrect result playing Feast in hand with 3 coppers ");
      printf("to try gaining a cost 6 card.\n");
   }
   else {
      res1 = compStates6(copyState, state, p);
      res2 = checkFeastChanges(copyState, state, p);
      if((res1 == 0) || (res2 == 0)) {
         printf("###Incorrect result playing Feast in hand with 3 coppers ");
         printf("to try gaining a cost 6 card. See description above.\n");
      }
      else {
         printf("Correct result playing Feast in hand with 3 coppers to ");
         printf("unsuccessfully gain a cost 6 card.\n");
      }
   }
   // Test Feast to gain a card outside of the supply on the high end
   resetStates6(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], handNoTreas, sizeof(handNoTreas));
   state.handCount[p] = (sizeof(handNoTreas) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(3, treasure_map + 1, 0, 0, &state);
   if(res != -1) {
      printf("###Incorrect result playing Feast to try to gain a card ");
      printf("greater than max supply card.\n");
   }
   else {
      printf("Correct result playing Feast to unsuccessfully try to ");
      printf("gain a card greater than max supply card.\n");
   }
   // Test Feast to gain a card outside of the supply on the low end
   resetStates6(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], handNoTreas, sizeof(handNoTreas));
   state.handCount[p] = (sizeof(handNoTreas) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(3, -1, 0, 0, &state);
   if(res != -1) {
      printf("###Incorrect result playing Feast to try to gain a card ");
      printf("with id value less than zero.\n");
   }
   else {
      printf("Correct result playing Feast to unsuccessfully try to ");
      printf("gain a card with id value less than zero.\n");
   }
}

int  main(int argc, char *argv[]) {
   testFeast();
   return 0;
}
