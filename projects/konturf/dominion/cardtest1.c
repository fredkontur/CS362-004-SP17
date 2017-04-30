/******************************************************************************
 * Filename:     cardtest1.c
 * Author:       Fred Kontur
 * Date Written: April 23, 2016
 * Last Edited:  April 23, 2016
 * Description:  This file contains a unit test for the smithy card  
 *               implementaton in dominion.c
 * Business Requirements for smithy card implementation:
 * 1. The player gets 3 cards in their hand from his/her deck, using up one of
 *    the actions for the turn.
 * 2. The player discards smithy after #1 is completed.
 * 3. If 3 cards are not available from the player's deck, the discard is
 *    shuffled and the player draws any remaining cards from there.
 * 4. No other gameState variables are changed except those required to 
 *    complete actions 1-3 above.
 * 5. Smithy can only be played when the player has 1 or more actions
 *    remaining.
******************************************************************************/
#include "dominion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_NUM_PLAYERS 4
#define TEST_PLAYER 0

// Returns 1 if decks are the same, 0 if different
int compDecks4(int* arr1, int* arr2, int deckSize) {
   int i;

   for(i = 0; i < deckSize; i++) {
      if(arr1[i] != arr2[i]) {
         return 0;
      }
   }

   return 1;
}

// This function compares 2 game states. It returns 1 if the states are the 
// same, 0 if different. This comparison excludes the hand, deck, discard, and 
// played cards pile of the current player and the numActions counter.
int compStates4(struct gameState s1, struct gameState s2, int currPlayer) {
   int i, res = 1;

   if((s1.numPlayers) != (s2.numPlayers)) {
      printf("Smithy changed the number of players in the game.\n");
      res = 0;
   }
   for(i = 0; i <= treasure_map; i++) {
      if((s1.supplyCount[i]) != (s2.supplyCount[i])) {
         printf("Smithy changed the number of cards in"); 
         printf(" the supply deck.\n");
         res = 0;
      }
      if((s1.embargoTokens[i]) != (s2.embargoTokens[i])) {
         printf("Smithy changed the number of embargo tokens.\n");
         res = 0;
      }
   }
   if((s1.outpostPlayed) != (s2.outpostPlayed)) {
      printf("Smithy changed the count of outposts played.\n");
      res = 0;
   }
   if((s1.whoseTurn) != (s2.whoseTurn)) {
      printf("Smithy changed whose turn it is.\n");
      res = 0;
   }
   if((s1.phase) != (s2.phase)) {
      printf("Smithy changed the phase variable.\n");
      res = 0;
   }
   if((s1.numBuys) != (s2.numBuys)) {
      printf("Smithy changed the buys counter.\n");
      res = 0;
   }
   for(i = 0; i < s1.numPlayers; i++) {
      if(i != currPlayer) {
         if((s1.deckCount[i]) != (s2.deckCount[i])) {
            printf("Smithy changed the number of cards in the deck of"); 
            printf("  Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks4(s1.deck[i], s2.deck[i], s1.deckCount[i]))) {
            printf("Smithy changed the cards in the deck");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if((s1.handCount[i]) != (s2.handCount[i])) {
            printf("Smithy changed the number of cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks4(s1.hand[i], s2.hand[i], s1.handCount[i]))) {
            printf("Smithy changed the cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if((s1.discardCount[i]) != (s2.discardCount[i])) {
            printf("Smithy changed the number of cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks4(s1.discard[i], s2.discard[i], s1.discardCount[i]))) {
            printf("Smithy changed the cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
      }
   }
   return res;
}

// Checks to see if a card is in the passed deck
int isCardInDeck(int *arr, int card, int numCards) {
   int i;

   for(i = 0; i < numCards; i++) {
      if(arr[i] == card) {
         return 1;
      }
   }

   return 0;
}

// Checks the changes in gameState that should have been caused by the 
// Smithy card. If the changes are valid, it retuns 1. If not, it retruns 0.
int checkSmithyChanges(struct gameState originalState, 
                       struct gameState finalState, int currPlayer)
{
   int i, j, n, card, res = 1;
   int orgnlDckCnt = originalState.deckCount[currPlayer];
   int fnlDckCnt = finalState.deckCount[currPlayer];
   int orgnlHndCnt = originalState.handCount[currPlayer];
   int fnlHndCnt = finalState.deckCount[currPlayer];
   int orgnlDscrdCnt = originalState.discardCount[currPlayer];
   int orgnlPlydCnt = originalState.playedCardCount;
   int fnlPlydCnt = finalState.playedCardCount;
   int *adjDeck, *adjHand;
   int m = fnlDckCnt;

   if((originalState.numActions - 1) != (finalState.numActions)) {
      printf("numActions is incorrect after Smithy played\n");
      res = 0;
   }
   if((orgnlHndCnt + 3) != fnlHndCnt) { 
      printf("Player %d hand count is incorrect after ", currPlayer);
      printf("Smithy played\n");
      res = 0;
   }
   else {
      if(orgnlDckCnt >= 3) {
         i = 3;
         if(orgnlDckCnt != (fnlDckCnt + 3)) {
            printf("Player %d deck count incorrect after Smithy ", currPlayer);
            printf("action\n");
            res = 0;
         }
         if(!(compDecks4(originalState.discard[currPlayer], 
                   finalState.discard[currPlayer], orgnlDscrdCnt)))
         {
            printf("Smithy incorrectly changed the discard deck of the ");
            printf("current player\n");
            res = 0;
         }
      }
      else {
         i = orgnlDckCnt;
         n = orgnlDscrdCnt;
         m = 0;
         // Check to make sure that, if discard deck needed to be shuffled
         // in order to draw cards for Smithy, that it was cards from the 
         // discard deck that ended up in the Player's hand.
         for(j = 3; j <= i; j--) {
            card = finalState.hand[currPlayer][orgnlHndCnt + j - 1];
            if(!(isCardInDeck(originalState.discard[currPlayer], card, n))) {
               printf("Discard cards needed to be shuffled for drawing ");
               printf("cards for Smithy, but this did not occur correctly\n");
               res = 0; 
            }
         }
      }
      // Make sure the cards were properly chosen from the deck and placed
      // into the player's hand
      adjDeck = &(originalState.deck[currPlayer][m]);
      adjHand = &(finalState.hand[currPlayer][orgnlHndCnt]);
      if(!compDecks4(adjDeck, adjHand, i)) {
         printf("Player %d did not properly select cards ", currPlayer);
         printf("from deck in Smithy action\n");
         res = 0;
      }
   }
   if(((orgnlPlydCnt + 1) != fnlPlydCnt) || 
      (finalState.playedCards[orgnlPlydCnt] != smithy)) 
   {
      printf("Smithy was not properly discarded into the played ");
      printf("card deck\n");
      res = 0;
   }
   return res;
}

void resetStates4(struct gameState *state1, struct gameState *state2, 
                 int numPlayer, int* k, int seed) {
   // First, clear the states
   memset(state1, 0, sizeof(struct gameState));
   memset(state2, 0, sizeof(struct gameState));

   // Now, initialize one of the states and then copy it
   initializeGame(numPlayer, k, seed, state1);
   memcpy(state2, state1, sizeof(struct gameState));
}

void testSmithy() {
   int numPlayer = TEST_NUM_PLAYERS, p = TEST_PLAYER, res, res1, res2;
   struct gameState state, copyState;
   int seed = 123456;

   // Create an array of kingdom cards
   int k[] = {adventurer, council_room, feast, gardens, mine,
              remodel, smithy, village, baron, great_hall};

   // Create a dummy discard pile
   int dscrd[] = {copper, silver, estate, sea_hag, minion};

   printf("\n***************Tests for Smithy Card***************\n");
   printf("*Test Conditions: #Players = %d, ", numPlayer);
   printf("Current Player = %d*\n", p);

   // Test Smithy when there are more than 3 cards in the original deck
   resetStates4(&state, &copyState, numPlayer, k, seed);
   state.hand[p][state.handCount[p]] = smithy;
   (state.handCount[p])++;
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(state.handCount[p] - 1, 0, 0, 0, &state);
   if(res != 0) {
      printf("###Incorrect result playing Smithy with more than 3 cards in");
      printf("the original deck.\n");
   }
   else {
      res1 = compStates4(copyState, state, p);
      res2 = checkSmithyChanges(copyState, state, p);
      if((res1 == 0) || (res2 == 0)) {
         printf("###Incorrect result playing Smithy with more than 3 cards ");
         printf("in the original deck. See description above\n");
      }
      else {
         printf("Correct result playing Smithy with more than 3 cards in ");
         printf("the original deck.\n");
      }
   }
   // Test Smithy when there are fewer than 3 cards in the original deck
   resetStates4(&state, &copyState, numPlayer, k, seed);
   state.hand[p][state.handCount[p]] = smithy;
   (state.handCount[p])++;
   state.deckCount[p] = 2;
   memcpy(state.discard[p], dscrd, sizeof(dscrd));
   state.discardCount[p] = (sizeof(dscrd) / sizeof(int));
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(state.handCount[p] - 1, 0, 0, 0, &state);
   if(res != 0) {
      printf("###Incorrect result playing Smithy with fewer than 3 cards in");
      printf("the original deck.\n");
   }
   else {
      res1 = compStates4(copyState, state, p);
      res2 = checkSmithyChanges(copyState, state, p);
      if((res1 == 0) || (res2 == 0)) {
         printf("###Incorrect result playing Smithy with fewer than 3 cards ");
         printf("in the original deck. See description above\n");
      }
      else {
         printf("Correct result playing Smithy with fewer than 3 cards in ");
         printf("the original deck.\n");
      }
   }
   // Test Smithy when the current player doesn't have any actions left.
   resetStates4(&state, &copyState, numPlayer, k, seed);
   state.hand[p][state.handCount[p]] = smithy;
   (state.handCount[p])++;
   state.numActions = 0;
   memcpy(&copyState, &state, sizeof(struct gameState));
   res = playCard(state.handCount[p] - 1, 0, 0, 0, &state);
   if(res != -1) {
      printf("###Incorrect result - player was allowed to play Smithy with");
      printf("no actions left.\n");
   }
   else {
      printf("Correct result - player was not allowed to play Smithy ");
      printf("because he/she had no actions left.\n");
   }
}