/******************************************************************************
 * Filename:     cardtest4.c
 * Author:       Fred Kontur
 * Date Written: April 25, 2016
 * Last Edited:  April 29, 2016
 * Description:  This file contains a unit test for the Mine card in dominion.c 
 * Business Requirements for isGameOver():
 * 1. Using the Mine allows the player to trash one of his/her Treasure cards
 *    and gain a Treasure card costing up to 3 more.
 * 2. One action is used in doing the above.
 * 3. The Mine card goes into the played card pile when it is played.
 * 4. If the player has no Treasure in his/her hand to trash, then no Treasure
 *    can be gained.
 * 5. No other game state variables are changed except those required to 
 *    accomplish #1.
******************************************************************************/
#include "dominion.h"
#include "dominion_helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_NUM_PLAYERS 3
#define TEST_PLAYER 2

int compareMine(const void* a, const void* b) {
   if(*(int*)a > *(int*)b) {
      return 1;
   }
   else if(*(int*)a < *(int*)b) {
      return -1;
   }
   else {
      return 0;
   }
}

// Returns 1 if decks are the same, 0 if different
int compDecks7(int* arr1, int* arr2, int deckSize) {
   int i;

   for(i = 0; i < deckSize; i++) {
      if(arr1[i] != arr2[i]) {
         return 0;
      }
   }

   return 1;
}

// This function compares 2 game states with the exception of the numActions
// counter, the Treasure cards in the supply, the played card deck and counter,
// and the current player's hand and handCount. It returns 1 if the states are 
// the same, 0 if different.
int compStates7(struct gameState s1, struct gameState s2, int currPlayer) {
   int i, res = 1;

   if((s1.numPlayers) != (s2.numPlayers)) {
      printf("Mine changed the number of players in the game.\n");
      res = 0;
   }
   for(i = 0; i <= treasure_map; i++) {
      if((i != copper) && (i != silver) && (i != gold)) {
         if((s1.supplyCount[i]) != (s2.supplyCount[i])) {
            printf("Mine changed the number of non-Treasure cards in"); 
            printf(" the supply deck.\n");
            res = 0;
         }
      }
      if((s1.embargoTokens[i]) != (s2.embargoTokens[i])) {
         printf("Mine changed the number of embargo tokens.\n");
         res = 0;
      }
   }
   if((s1.outpostPlayed) != (s2.outpostPlayed)) {
      printf("Mine changed the count of outposts played.\n");
      res = 0;
   }
   if((s1.whoseTurn) != (s2.whoseTurn)) {
      printf("Mine changed whose turn it is.\n");
      res = 0;
   }
   if((s1.phase) != (s2.phase)) {
      printf("Mine changed the phase variable.\n");
      res = 0;
   }
   if((s1.numBuys) != (s2.numBuys)) {
      printf("Mine changed the buys counter.\n");
      res = 0;
   }
   for(i = 0; i < s1.numPlayers; i++) {
      if((s1.deckCount[i]) != (s2.deckCount[i])) {
         printf("Mine changed the number of cards in the deck of"); 
         printf("  Player %d.\n", i);
         res = 0;
      }
      if(!(compDecks7(s1.deck[i], s2.deck[i], s1.deckCount[i]))) {
         printf("Mine changed the cards in the deck");
         printf(" of Player %d.\n", i);
         res = 0;
      }
      if(i != currPlayer) {
         if((s1.handCount[i]) != (s2.handCount[i])) {
         printf("Mine changed the number of cards in the hand");
         printf(" of Player %d.\n", i);
         res = 0;
         }
         if(!(compDecks7(s1.hand[i], s2.hand[i], s1.handCount[i]))) {
            printf("Mine changed the cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if((s1.discardCount[i]) != (s2.discardCount[i])) {
            printf("Mine changed the number of cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks7(s1.discard[i], s2.discard[i], s1.discardCount[i]))) {
            printf("Mine changed the cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
      }
   }
   return res;
}

// Checks the changes in gameState that should have been caused by the Mine 
// card. If the changes are valid, it returns 1. If not, it returns 0.
int checkMineChanges(struct gameState originalState,
                     struct gameState finalState, int currPlayer)
{
   int i, j, n = 0, res = 1, numDiff = 0, chosenCard, trashedCard;
   int sOrgnl, sFnl, cOrgnl, cFnl;
   int orgnlHndCnt = originalState.handCount[currPlayer];
   int fnlHndCnt = finalState.handCount[currPlayer];
   int orgnlPlydCnt = originalState.playedCardCount;
   int fnlPlydCnt = finalState.playedCardCount;

   if(originalState.numActions != (finalState.numActions + 1)) {
      printf("numActions is incorrect after Mine played\n");
      res = 0;
   }
   if((orgnlPlydCnt + 1) != fnlPlydCnt) {
      printf("Played card deck count is incorrect after Mine card is played\n");
      res = 0;
   }
   if(finalState.playedCards[orgnlPlydCnt] != mine) {
      printf("Mine card was not properly discarded to the played card deck\n");
   }
   // Determine which treasure card was chosen from the supply
   for(i = copper; i <= gold; i++) {
      sOrgnl = originalState.supplyCount[i];
      sFnl = finalState.supplyCount[i];
      if(sOrgnl != sFnl) {
         if(sOrgnl < sFnl) {
            printf("Treasure Card #%d count in supply is incorrect ", i);
            printf("after Mine is played\n");
            res = 0;
            break; 
         }
         else {
            numDiff = sOrgnl - sFnl;
            chosenCard = i;
         }
      }
   }
   if(numDiff != 1) {
      printf("More than 1 Treasure card was taken from the supply\n");
      res = 0;
   }
   // Determine which card in the hand changed
   if(orgnlHndCnt != (fnlHndCnt + 1)) {
      printf("Hand count is incorrect after Mine card is played\n");
      res = 0;
   }
   else {
      // Sort the hands to make it easier to compare them
      qsort((void *)originalState.hand[currPlayer], orgnlHndCnt, sizeof(int), 
            compareMine);
      qsort((void *)finalState.hand[currPlayer], fnlHndCnt, sizeof(int),
            compareMine);
      i = j = 0;
      while(i < orgnlHndCnt) {
         while(j < fnlHndCnt) {
            cOrgnl = originalState.hand[currPlayer][i];
            cFnl = finalState.hand[currPlayer][j];
            if(cOrgnl != mine) {
               if(cOrgnl != cFnl) {
                  if((cFnl != chosenCard) || (n >= 1)) {
                     printf("The current player's hand has an incorrect card ");
                     printf("after the Mine card is played\n");
                     res = 0;
                     break;
                  }
                  else {
                     i++;
                     j++;
                     n++;
                     trashedCard = cOrgnl;
                  }
               }
               else {
                  i++;
                  j++;
               }
            }
            else {
               i++;
            }   
         }
      }
   }
   // Make sure the chosen card only costs up to 3 more than the trashed card
   if(getCost(chosenCard) > (getCost(trashedCard) + 3)) {
      printf("Chosen card cost greater than 3 more than the trashed card\n");
      res = 0;
   }
   return res;
}

void resetStates7(struct gameState *state1, struct gameState *state2, 
                 int numPlayer, int* k, int seed) {
   // First, clear the states
   memset(state1, 0, sizeof(struct gameState));
   memset(state2, 0, sizeof(struct gameState));

   // Now, initialize one of the states and then copy it
   initializeGame(numPlayer, k, seed, state1);
   memcpy(state2, state1, sizeof(struct gameState));
}

void testMine() {
   int p = TEST_PLAYER, numPlayer = TEST_NUM_PLAYERS, res, res1, res2;
   struct gameState state, copyState;
   int seed = 123456;

   // Create an array of kingdom cards
   int k[] = {adventurer, council_room, feast, gardens, mine,
              remodel, smithy, village, baron, great_hall};
   // Create several different hands
   int hCopper[] = {copper, sea_hag, mine, province};
   int hSilver[] = {village, duchy, smithy, silver, mine};

   printf("\n***************Tests for Mine***************\n");

   // Test using Mine to trash copper and gain silver
   resetStates7(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], hCopper, sizeof(hCopper));
   state.handCount[p] = (sizeof(hCopper) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(2, 0, silver, 0, &state);
   if(res != 0) {
      printf("###Incorrect result when using Mine to trash copper and ");
      printf("gain silver.\n");
   }
   else {
      res1 = compStates7(copyState, state, p);
      res2 = checkMineChanges(copyState, state, p);
      if((res1 == 0) || (res2 == 0)) {
         printf("###Incorrect result when using Mine to trash copper and ");
         printf("gain silver. See description above.\n");
      }
      else {
         printf("Correct result when using Mine to trash copper and ");
         printf("gain silver.\n");
      }
   }
   // Test using Mine to trash copper and try to gain gold
   resetStates7(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], hCopper, sizeof(hCopper));
   state.handCount[p] = (sizeof(hCopper) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(2, 0, gold, 0, &state);
   if(res != -1) {
      printf("###Incorrect result when using Mine to trash copper and ");
      printf("gain gold.\n");
   }
   else {
      printf("Correct result when using Mine to unsuccessfully try to trash ");
      printf("copper and gain gold.\n");
   }
   // Test using Mine to trash copper and gain non-Treasure card
   resetStates7(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], hCopper, sizeof(hCopper));
   state.handCount[p] = (sizeof(hCopper) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(2, 0, steward, 0, &state);
   if(res != -1) {
      printf("###Incorrect result when using Mine to ");
      printf("gain non-Treasure card.\n");
   }
   else {
      printf("Correct result when using Mine to unsuccessfully try to gain ");
      printf("non-Treasure card.\n");
   }
   // Test using Mine to gain card outside of the range of supply on high end
   resetStates7(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], hCopper, sizeof(hCopper));
   state.handCount[p] = (sizeof(hCopper) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(2, 0, treasure_map + 1, 0, &state);
   if(res != -1) {
      printf("###Incorrect result when using Mine to ");
      printf("gain card greater than the range of supply.\n");
   }
   else {
      printf("Correct result when using Mine to unsuccessfully try to gain ");
      printf("card greater than the range of supply.\n");
   }
   // Test using Mine to gain card outside of the range of supply on low end
   resetStates7(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], hCopper, sizeof(hCopper));
   state.handCount[p] = (sizeof(hCopper) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(2, 0, -1, 0, &state);
   if(res != -1) {
      printf("###Incorrect result when using Mine to ");
      printf("gain card with id value -1.\n");
   }
   else {
      printf("Correct result when using Mine to unsuccessfully try to gain ");
      printf("card with id value -1.\n");
   }
   // Test using Mine to trash sea_hag and gain silver
   resetStates7(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], hCopper, sizeof(hCopper));
   state.handCount[p] = (sizeof(hCopper) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(2, 1, silver, 0, &state);
   if(res != -1) {
      printf("###Incorrect result when using Mine to trash sea_hag ");
      printf("and gain silver.\n");
   }
   else {
      printf("Correct result when using Mine to unsuccessfully try to trash ");
      printf("sea_hag and gain silver.\n");
   }
   // Test using Mine to trash province and gain silver
   resetStates7(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], hCopper, sizeof(hCopper));
   state.handCount[p] = (sizeof(hCopper) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(2, 3, silver, 0, &state);
   if(res != -1) {
      printf("###Incorrect result when using Mine to trash province ");
      printf(" and gain silver.\n");
   }
   else {
      printf("Correct result when using Mine to unsuccessfully try to trash ");
      printf("province and gain silver.\n");
   }
   // Test using Mine to trash card with handPos = -1
   resetStates7(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], hCopper, sizeof(hCopper));
   state.handCount[p] = (sizeof(hCopper) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(-1, 1, silver, 0, &state);
   if(res != -1) {
      printf("###Incorrect result when using Mine to trash card with ");
      printf("handPos = -1.\n");
   }
   else {
      printf("Correct result when using Mine to unsuccessfully try to trash ");
      printf("card with handPos = -1.\n");
   }
   // Test using Mine to trash card with handPos = handCount
   resetStates7(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], hCopper, sizeof(hCopper));
   state.handCount[p] = (sizeof(hCopper) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(state.handCount[p], 1, silver, 0, &state);
   if(res != -1) {
      printf("###Incorrect result when using Mine to trash card with ");
      printf("handPos = handCount.\n");
   }
   else {
      printf("Correct result when using Mine to unsuccessfully try to trash ");
      printf("card with handPos = handCount.\n");
   }
   // Test using Mine to trash silver and gain gold
   resetStates7(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = p;
   memcpy(state.hand[p], hSilver, sizeof(hSilver));
   state.handCount[p] = (sizeof(hSilver) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(4, 3, gold, 0, &state);
   if(res != 0) {
      printf("###Incorrect result when using Mine to trash silver and ");
      printf("gain gold.\n");
   }
   else {
      res1 = compStates7(copyState, state, p);
      res2 = checkMineChanges(copyState, state, p);
      if((res1 == 0) || (res2 == 0)) {
         printf("###Incorrect result when using Mine to trash silver and ");
         printf("gain gold. See description above.\n");
      }
      else {
         printf("Correct result when using Mine to trash silver and ");
         printf("gain gold.\n");
      }
   }
}
