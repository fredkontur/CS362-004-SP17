/******************************************************************************
 * Filename:     cardtest2.c
 * Author:       Fred Kontur
 * Date Written: April 23, 2017
 * Last Edited:  May 20, 2017
 * Description:  This file contains a unit test for the Adventurer card 
 *               in dominion.c
 * Business Requirements for Adventurer:
 * 1. The player uses an action to draw cards from the deck until 2 Treasure 
 *    cards are found. The following caveats apply -
 *    a) The Treasure cards are placed in the player's hand while all 
 *       non-Treasure cards that are found are discarded.
 *    b) If it is necessary to shuffle in the middle, then the player should
 *       do so.
 *    c) If the player runs out of cards and still only has one treasure, then
 *       the player only gets one treasure.
 * 2. Nothing else changes in the game state except what is required to 
 *    accomplish the above actions. 
 * 3. If the player does not have actions available, then he/she cannot use
 *    the card.
******************************************************************************/
#include "dominion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_NUM_PLAYERS 3
#define TEST_PLAYER 0

// Returns 1 if decks are the same, 0 if different
int compDecks5(int* arr1, int* arr2, int deckSize) {
   int i;

   for(i = 0; i < deckSize; i++) {
      if(arr1[i] != arr2[i]) {
         return 0;
      }
   }

   return 1;
}

// This function compares 2 game states with the exception of the current 
// player's hand, deck, played cards, and discard pile and the corresponding 
// counts. It also does not compare numActions and coins. It returns 1 if the 
// states are the same, 0 if different.
int compStates5(struct gameState s1, struct gameState s2, int currPlayer) {
   int i, res = 1;

   if((s1.numPlayers) != (s2.numPlayers)) {
      printf("Adventurer changed the number of players in the game.\n");
      res = 0;
   }
   for(i = 0; i <= treasure_map; i++) {
      if((s1.supplyCount[i]) != (s2.supplyCount[i])) {
         printf("Adventurer changed the number of cards in"); 
         printf(" the supply deck.\n");
         res = 0;
      }
      if((s1.embargoTokens[i]) != (s2.embargoTokens[i])) {
         printf("Adventurer changed the number of embargo tokens.\n");
         res = 0;
      }
   }
   if((s1.outpostPlayed) != (s2.outpostPlayed)) {
      printf("Adventurer changed the count of outposts played.\n");
      res = 0;
   }
   if((s1.whoseTurn) != (s2.whoseTurn)) {
      printf("Adventurer changed whose turn it is.\n");
      res = 0;
   }
   if((s1.phase) != (s2.phase)) {
      printf("Adventurer changed the phase variable.\n");
      res = 0;
   }
   if((s1.numBuys) != (s2.numBuys)) {
      printf("Adventurer changed the buys counter.\n");
      res = 0;
   }
   for(i = 0; i < s1.numPlayers; i++) {
      if(i != currPlayer) {
         if((s1.deckCount[i]) != (s2.deckCount[i])) {
            printf("Adventurer changed the number of cards in the deck of"); 
            printf("  Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks5(s1.deck[i], s2.deck[i], s1.deckCount[i]))) {
            printf("Adventurer changed the cards in the deck");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if((s1.handCount[i]) != (s2.handCount[i])) {
            printf("Adventurer changed the number of cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks5(s1.hand[i], s2.hand[i], s1.handCount[i]))) {
            printf("Adventurer changed the cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if((s1.discardCount[i]) != (s2.discardCount[i])) {
            printf("Adventurer changed the number of cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks5(s1.discard[i], s2.discard[i], s1.discardCount[i]))) {
            printf("Adventurer changed the cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
      }
   }
   return res;
}

int checkAdventurerChanges(struct gameState originalState,
                           struct gameState finalState, int currPlayer)
{
   int i, j, diff, currCard, orgnlTCnt = 0, fnlTCnt = 0, res = 1;
   int treasTot = 0;
   int orgnlHndCnt = originalState.handCount[currPlayer];
   int fnlHndCnt = finalState.handCount[currPlayer];
   int orgnlPlydCnt = originalState.playedCardCount;
   int fnlPlydCnt = finalState.playedCardCount;
   int orgnlDckCnt = originalState.deckCount[currPlayer];
   int fnlDckCnt = finalState.deckCount[currPlayer];
   int orgnlDscrdCnt = originalState.discardCount[currPlayer];
   int fnlDscrdCnt = finalState.discardCount[currPlayer];

   // Check if an action was used
   if(originalState.numActions != (finalState.numActions + 1)) {
      printf("Using Adventurer did not take away an action as it should\n");
      res = 0;
   }
   // Check if Adventurer was discarded in PlayedCard deck
   if((orgnlPlydCnt + 1) != fnlPlydCnt) {
      printf("Adventurer was not properly discarded in PlayedCard deck\n");
      res = 0;
   }
   else {
      if(finalState.playedCards[orgnlPlydCnt] != adventurer) {
         printf("Adventurer was not properly discarded in PlayedCard deck\n");
         res = 0;
      }
   }
   // Count the Treasure cards in the original deck
   for(i = 0; i < originalState.deckCount[currPlayer]; i++) {
      currCard = originalState.deck[currPlayer][i];
      if((currCard == copper) || (currCard == silver) || (currCard == gold)) {
         orgnlTCnt++;
      }
   }
   for(i = 0; i < originalState.discardCount[currPlayer]; i++) {
      currCard = originalState.discard[currPlayer][i];
      if((currCard == copper) || (currCard == silver) || (currCard == gold)) {
         orgnlTCnt++;
      }
   }
   // Count the Treasure cards in the final deck
   for(i = 0; i < finalState.deckCount[currPlayer]; i++) {
      currCard = finalState.deck[currPlayer][i];
      if((currCard == copper) || (currCard == silver) || (currCard == gold)) {
         fnlTCnt++;
      }
   }
   for(i = 0; i < finalState.discardCount[currPlayer]; i++) {
      currCard = finalState.discard[currPlayer][i];
      if((currCard == copper) || (currCard == silver) || (currCard == gold)) {
         fnlTCnt++;
      }
   }
   // See if the correct number of Treasure cards were added to the player's
   // hand from the deck/discard pile
   if(orgnlTCnt >= 2) {
      if(orgnlTCnt != (fnlTCnt + 2)) {
         printf("2 Treasures were not removed from deck and discard piles ");
         printf("as should have occured.\n");
         res = 0;
      }
      if((orgnlHndCnt + 1) != fnlHndCnt) {
         printf("Current player did not pick up 2 Treasures ");
         printf("and discard non-Treasure cards as required\n");
         res = 0;
      }
   }
   else {
      if(fnlTCnt != 0) {
         printf("Treasures were not removed from deck and discard ");
         printf("piles as should have occurred.\n");
         res = 0;
      }
      if((orgnlHndCnt + orgnlTCnt - 1) != fnlHndCnt) {
         printf("Current player did not pick up %d Treasure ", orgnlTCnt);
         printf("and discard non-Treasure cards as required\n");
         res = 0;
      }
   }
   // Check that the correct number of non-Treasure cards were moved from 
   // deck/discard to the discard pile
   diff =  (orgnlDckCnt + orgnlDscrdCnt) - (fnlDckCnt + fnlDscrdCnt); 
   if(diff != (orgnlTCnt - fnlTCnt)) {
      printf("Non-Treasure cards were not discarded as should have ");
      printf("occurred\n");
      res = 0;
   }
   // Count Treasure in hand for final state
   for(i = 0; i < fnlHndCnt; i++) {
      currCard = finalState.hand[currPlayer][i];
      if(currCard == copper) {
         treasTot += 1;
      }
      else if(currCard == silver) {
         treasTot += 2;
      }
      else if(currCard == gold) {
         treasTot += 3;
      }
   }
   // Determine if coins were properly updated
   if(finalState.coins != treasTot) {
      printf("Coins were not properly updated after Adventurer was played\n");
      res = 0;
   }
   return res;
}

void resetStates5(struct gameState *state1, struct gameState *state2, 
                 int numPlayer, int* k, int seed) {
   // First, clear the states
   memset(state1, 0, sizeof(struct gameState));
   memset(state2, 0, sizeof(struct gameState));

   // Now, initialize one of the states and then copy it
   initializeGame(numPlayer, k, seed, state1);
   memcpy(state2, state1, sizeof(struct gameState));
}

void testAdventurer() {
   int numPlayer = TEST_NUM_PLAYERS, p = TEST_PLAYER, res, res1, res2;
   struct gameState state, copyState;
   int seed = 123456;

   // Create an array of kingdom cards
   int k[] = {adventurer, council_room, feast, gardens, mine,
              remodel, smithy, village, baron, great_hall};
   // Create arrays of deck cards
   int deck0Treas[] = {estate, province, minion, estate};
   int deck1Treas[] = {smithy, copper, gardens, village, sea_hag};
   int deck3Treas[] = {gold, baron, silver, copper, duchy, remodel};
   // Create a discard deck
   int discard1Treas[] = {village, gold};

   printf("\n***************Tests for Adventurer Card***************\n");
   printf("***Test Conditions: #Players = %d, ", numPlayer);
   printf("Current Player = %d***\n", p);

   // Test Adventurer for a game with 3 Treasures in deck
   resetStates5(&state, &copyState, numPlayer, k, seed);
   memcpy(state.deck[p], deck3Treas, sizeof(deck3Treas));
   state.deckCount[p] = (sizeof(deck3Treas) / sizeof(int));
   state.hand[p][state.handCount[p]] = adventurer;
   (state.handCount[p])++;
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(state.handCount[p] - 1, 0, 0, 0, &state);
   if(res == -1) {
      printf("###Incorrect result for Adventurer with a deck with 3 ");
      printf("Treasures.\n");
   }
   else {
      res1 = compStates5(copyState, state, p);
      res2 = checkAdventurerChanges(copyState, state, p);
      if((res1 == 0) || (res2 == 0)) {
         printf("###Incorrect result for Adventurer with a deck with 3 ");
         printf("Treasures. See description above.\n");

      }
   }
   // Test Adventurer for a game with 1 Treasure in deck and 1 Treasure in
   // discard
   resetStates5(&state, &copyState, numPlayer, k, seed);
   memcpy(state.deck[p], deck1Treas, sizeof(deck1Treas));
   state.deckCount[p] = (sizeof(deck1Treas) / sizeof(int));
   memcpy(state.discard[p], discard1Treas, sizeof(discard1Treas));
   state.discardCount[p] = (sizeof(discard1Treas) / sizeof(int));
   state.hand[p][state.handCount[p]] = adventurer;
   (state.handCount[p])++;
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(state.handCount[p] - 1, 0, 0, 0, &state);
   if(res == -1) {
      printf("###Incorrect result for Adventurer with a deck and discard ");
      printf("with 1 Treasure each.\n");
   }
   else {
      res1 = compStates5(copyState, state, p);
      res2 = checkAdventurerChanges(copyState, state, p);
      if((res1 == 0) || (res2 == 0)) {
         printf("###Incorrect result for Adventurer with a deck and discard ");
         printf("with 1 Treasure each. See description above.\n");
      }
   }
   // Test Adventurer for a game with 0 Treasure in deck and 1 Treasure in
   // discard
   resetStates5(&state, &copyState, numPlayer, k, seed);
   memcpy(state.deck[p], deck0Treas, sizeof(deck0Treas));
   state.deckCount[p] = (sizeof(deck0Treas) / sizeof(int));
   memcpy(state.discard[p], discard1Treas, sizeof(discard1Treas));
   state.discardCount[p] = (sizeof(discard1Treas) / sizeof(int));
   state.hand[p][state.handCount[p]] = adventurer;
   (state.handCount[p])++;
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(state.handCount[p] - 1, 0, 0, 0, &state);
   if(res == -1) {
      printf("###Incorrect result for Adventurer with a deck and discard ");
      printf("with a total of 1 Treasure.\n");
   }
   else {
      res1 = compStates5(copyState, state, p);
      res2 = checkAdventurerChanges(copyState, state, p);
      if((res1 == 0) || (res2 == 0)) {
         printf("###Incorrect result for Adventurer with a deck and discard ");
         printf("with a total of 1 Treasure. See description above.\n");
      }
   }
}

int main(int argc, char *argv[]) {
   testAdventurer();
   return 0;
}
