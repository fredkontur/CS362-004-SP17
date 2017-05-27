/******************************************************************************
 * Filename:     unittest3.c
 * Author:       Fred Kontur
 * Date Written: April 22, 2017
 * Last Edited:  May 20, 2017
 * Description:  This file contains a unit test for the endTurn() function in
 *               dominion.c
 * Business Requirements for endTurn():
 * 1. endTurn() should execute the final cleanup phase of the current player's 
 *    turn. In the cleanup phase, the player must discard all unplayed cards 
 *    and played cards back to the discard pile and draw again 5 new cards.
 * 2. endTurn() should change the gameState from the current player's turn to
 *    the beginning of the next player's turn.
 * 3. endTurn() should not change any other gameState variables except those
 *    required to execute 1 and 2 above.
******************************************************************************/
#include "dominion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare4(const void* a, const void* b) {
   if(*(int*)a > *(int*)b) {
      return 1;
   }
   else if(*(int*)a > *(int*)b) {
      return -1;
   }
   else {
      return 0;
   }
}

// Returns 1 if decks are the same, 0 if different
int compDecks2(int* arr1, int* arr2, int deckSize) {
   int i;

   for(i = 0; i < deckSize; i++) {
      if(arr1[i] != arr2[i]) {
         return 0;
      }
   }

   return 1;
}

// This function compares 2 game states with the exception of the following:
// - whoseTurn
// - outpostPlayed
// - phase
// - numActions
// - coins
// - numBuys
// - playedCards
// - playedCardCount
// - handCount[currPlayer]
// - hand[currPlayer]
// - handCount[nextPlayer]
// - hand[nextPlayer]
// - discardCount[currPlayer]
// - discard[currPlayer]
// It returns 1 if the states are the same, 0 if different.
int compStates2(struct gameState s1, struct gameState s2, int currPlayer) {
   int i, res = 1;

   if((s1.numPlayers) != (s2.numPlayers)) {
      printf("endTurn() changed the number of players in the game.\n");
      res = 0;
   }
   for(i = 0; i <= treasure_map; i++) {
      if((s1.supplyCount[i]) != (s2.supplyCount[i])) {
         printf("endTurn() changed the number of cards in"); 
         printf(" the supply deck.\n");
         res = 0;
      }
      if((s1.embargoTokens[i]) != (s2.embargoTokens[i])) {
         printf("endTurn() changed the number of embargo tokens.\n");
         res = 0;
      }
   }
   for(i = 0; i < s1.numPlayers; i++) {
      if(i != currPlayer) {
         if((s1.deckCount[i]) != (s2.deckCount[i])) {
            printf("endTurn() changed the number of cards in the deck of"); 
            printf(" Player %d.\n", i);
            res = 0;
         }
      }
      if(i != currPlayer) {
         if(!(compDecks2(s1.deck[i], s2.deck[i], s1.deckCount[i]))) {
            printf("endTurn() changed the cards in the deck");
            printf(" of Player %d.\n", i);
            res = 0;
         }
      }
      if(i != currPlayer) {
         if((s1.handCount[i]) != (s2.handCount[i])) {
            printf("endTurn() changed the number of cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks2(s1.hand[i], s2.hand[i], s1.handCount[i]))) {
            printf("endTurn() changed the cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
      }
      if(i != currPlayer) {
         if((s1.discardCount[i]) != (s2.discardCount[i])) {
            printf("endTurn() changed the number of cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks2(s1.discard[i], s2.discard[i], s1.discardCount[i]))) {
            printf("endTurn() changed the cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
      }
   }
   return res;
}

// This function compares two states to make sure that endTurn made the proper
// changes to them. It returns 1 if the changes are correct and 0 if not
int checkETChanges(struct gameState originalState, struct gameState finalState,
                   int currPlayer)
{
   int i, shift, currCard, res = 1;
   int numDiscarded = originalState.handCount[currPlayer];
   int numPlayed = originalState.playedCardCount;
   int dscrdCnt = originalState.discardCount[currPlayer];
   int dckCnt = originalState.deckCount[currPlayer];
   int hndCnt = originalState.handCount[currPlayer];

   if(finalState.outpostPlayed != 0) {
      printf("endTurn() did not correctly set outpostPlayed to 0\n");
      res = 0;
   }
   if(finalState.phase != 0) {
      printf("endTurn() did not correctly set the phase to 0\n");
      res = 0;
   }
   if(finalState.numActions != 1) {
      printf("endTurn() did not correctly set numActions to 1\n");
      res = 0;
   }
   if(finalState.numBuys != 1) {
      printf("endTurn() did not correctly set numBuys to 1\n");
      res = 0;
   }
   if(finalState.playedCardCount != 0) {
      printf("endTurn() did not correctly set playedCardCount to 0\n");
      res = 0;
   } 
   if(currPlayer < (originalState.numPlayers) - 1) {
      if(originalState.whoseTurn != (finalState.whoseTurn - 1)) {
         printf("endTurn() did not correctly change to the next player's ");
         printf("turn\n");
         res = 0;
      }
   }
   else {
      if(finalState.whoseTurn != 0) {
         printf("endTurn() did not correctly change to the next player's ");
         printf("turn\n");
         res = 0;
      }
   }
   // Make sure the current player properly discarded the cards in his/her 
   // hand
   if(dckCnt < 5) {
      if((finalState.discardCount[currPlayer]) != 0) {
         printf("Incorrect number of cards in discard pile for ");
         printf("Player %d\n", currPlayer);
         res = 0;
      } 
   }
   else {
      if((dscrdCnt + numDiscarded + numPlayed) != 
          finalState.discardCount[currPlayer])
      {
         printf("Incorrect number of cards in discard pile for ");
         printf("Player %d\n", currPlayer);
         res = 0;
      
      }
      else {
         // Add the hand cards and the played cards to the original state's 
         // discard piles
         for(i = 0; i < hndCnt; i++) {
            currCard = originalState.hand[currPlayer][i];
            originalState.discard[currPlayer][dscrdCnt + i] = currCard;
         }
         for(i = 0; i < originalState.playedCardCount; i++) {
            currCard = originalState.playedCards[i];
            shift = dscrdCnt + hndCnt + i;
            originalState.discard[currPlayer][shift] = currCard;
         }
         // Sort the original and final states' discard piles to compare them
         qsort((void*)originalState.discard[currPlayer], 
                finalState.discardCount[currPlayer], sizeof(int), compare4);
         qsort((void*)finalState.discard[currPlayer], 
                finalState.discardCount[currPlayer], sizeof(int), compare4);
         if(!(compDecks2(originalState.discard[currPlayer],
                        finalState.discard[currPlayer], currPlayer)))
         {
            printf("The cards from Player %d's hand and played ", currPlayer);
            printf("cards did not get properly discarded\n");
         }
      }
   }
   // Make sure the current player properly drew 5 cards from his/her deck
   if(finalState.handCount[currPlayer] != 5) {
      printf("Player %d did not draw 5 cards at the end of ", currPlayer);
      printf("the turn\n");
      res = 0;
   }
   else {
      if(dckCnt < 5) {
         for(i = 0; i < dckCnt; i++) {
            currCard = originalState.deck[currPlayer][dckCnt - 1 - i];
            if(finalState.hand[currPlayer][i] != currCard) {
               printf("Card %d was not properly moved from deck ", currCard);
               printf("player's hand\n");
               res = 0;
            }
         } 
      }
      else {
         for(i = 0; i < 5; i++) {
            currCard = originalState.deck[currPlayer][dckCnt - 1 - i];
            if(finalState.hand[currPlayer][i] != currCard) {
               printf("Card %d was not properly moved from deck ", currCard);
               printf("player's hand\n");
               res = 0;
            }
         }
      }
   }
   return res;
}

void resetStates2(struct gameState *state1, struct gameState *state2,
                 int numPlayer, int *k, int seed) 
{
   // First, clear the states
   memset(state1, 0, sizeof(struct gameState));
   memset(state2, 0, sizeof(struct gameState));

   // Now initialize one of the states and then copy it
   initializeGame(numPlayer, k, seed, state1);
   memcpy(state2, state1, sizeof(struct gameState));
}

void testEndTurn() {
   int numPlayer, currPlayer, res1, res2;
   struct gameState state, copyState;
   int seed = 123456;

   // Create an array of kingdom cards
   int k[] = {adventurer, council_room, feast, gardens, mine,
              remodel, smithy, village, baron, great_hall};

   // Create a dummy deck
   int deck[] = {copper, silver, minion, province};
   // Create a discard deck
   int discardDeck[] = {adventurer, salvager, gold, estate, duchy};

   printf("\n***************Tests for endTurn() function***************\n");
   
   // Test endTurn() for a 2-person game
   // Go from Player 0 to Player 1
   numPlayer = 2;
   currPlayer = 0;
   resetStates2(&state, &copyState, numPlayer, k, seed);
   endTurn(&state);
   res1 = compStates2(copyState, state, currPlayer);
   res2 = checkETChanges(copyState, state, currPlayer);
   if((res1 != 1) || (res2 != 1)) {
      printf("###Incorrect result for 2-person game going from Player 0 to ");
      printf("Player 1. See description above.\n");
   }
   else {
      printf("Correct resut for 2-person game going from Player 0 to ");
      printf("Player 1.\n");
   }
   // Go from Player 0 to Player 1 with played cards
   numPlayer = 2;
   currPlayer = 0;
   resetStates2(&state, &copyState, numPlayer, k, seed);
   state.playedCardCount = 4;
   memcpy(state.playedCards, deck, sizeof(deck));
   memcpy(&copyState, &state, sizeof(struct gameState));
   endTurn(&state);
   res1 = compStates2(copyState, state, currPlayer);
   res2 = checkETChanges(copyState, state, currPlayer);
   if((res1 != 1) || (res2 != 1)) {
      printf("###Incorrect result for 2-person game going from Player 0 to ");
      printf("Player 1 with played cards. See description above.\n");
   }
   else {
      printf("Correct resut for 2-person game going from Player 0 to ");
      printf("Player 1 with played cards.\n");
   }
   // Go from Player 1 to Player 0
   currPlayer = 1;
   memcpy(&copyState, &state, sizeof(struct gameState));
   endTurn(&state);
   res1 = compStates2(copyState, state, currPlayer);
   res2 = checkETChanges(copyState, state, currPlayer);
   if((res1 != 1) || (res2 != 1)) {
      printf("###Incorrect result for 2-person game going from Player 1 to ");
      printf("Player 0. See description above.\n");
   }
   else {
      printf("Correct resut for 2-person game going from Player 1 to ");
      printf("Player 0.\n");
   }

   // Test endTurn() for a 4-person game
   // Go from Player 1 to Player 2
   numPlayer = 4;
   currPlayer = 1;
   resetStates2(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = currPlayer;
   state.outpostPlayed = 5;
   state.numActions = 0;
   state.numBuys = 0;
   state.handCount[currPlayer] = 4;
   memcpy(state.hand[currPlayer], deck, sizeof(deck));
   memcpy(&copyState, &state, sizeof(struct gameState));
   endTurn(&state);
   res1 = compStates2(copyState, state, currPlayer);
   res2 = checkETChanges(copyState, state, currPlayer);
   if((res1 != 1) || (res2 != 1)) {
      printf("###Incorrect result for 4-person game going from Player 1 to ");
      printf("Player 2. See description above.\n");
   }
   else {
      printf("Correct resut for 4-person game going from Player 1 to ");
      printf("Player 2.\n");
   }
   // Go from Player 3 to Player 0
   currPlayer = 3;
   resetStates2(&state, &copyState, numPlayer, k, seed);
   state.whoseTurn = currPlayer;
   state.handCount[currPlayer] = 4;
   memcpy(state.hand[currPlayer], deck, sizeof(deck));
   state.discardCount[currPlayer] = 5;
   memcpy(state.discard[currPlayer], discardDeck, sizeof(discardDeck));
   memcpy(&copyState, &state, sizeof(struct gameState));
   endTurn(&state);
   res1 = compStates2(copyState, state, currPlayer);
   res2 = checkETChanges(copyState, state, currPlayer);
   if((res1 != 1) || (res2 != 1)) {
      printf("###Incorrect result for 4-person game going from Player 3 to ");
      printf("Player 0. See description above.\n");
   }
   else {
      printf("Correct resut for 4-person game going from Player 3 to ");
      printf("Player 0.\n");
   }
}

int main(int argc, char *argv[]) {
   testEndTurn();
   return 0;
}
