/******************************************************************************
 * Filename:     unittest1.c
 * Author:       Fred Kontur
 * Date Written: April 21, 2016
 * Last Edited:  April 27, 2016
 * Description:  This file contains a unit test for the shuffle() function in
 *               dominion.c
 * Business Requirements for shuffle():
 * 1. Shuffle should change the order of the cards in the current player's
 *    deck.
 * 2. Shuffle should not change the cards in the player's deck.
 * 3. Besides the order of the cards in the current player's deck, no other
 *    game state values should change.
 * 4. Shuffles of the same cards at different points in the game should 
 *    result in different card orders in the deck.
******************************************************************************/
#include "dominion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_NUM_PLAYERS 4
#define TEST_PLAYER 2

int compare2(const void* a, const void* b) {
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
int compDecks1(int* arr1, int* arr2, int deckSize) {
   int i;

   for(i = 0; i < deckSize; i++) {
      if(arr1[i] != arr2[i]) {
         return 0;
      }
   }

   return 1;
}

// Returns 1 if the same cards are in the two decks (irrespective of order) 
// and 0 if there are one or more different cards in the decks
int sameCards(int* arr1, int* arr2, int deckSize) {
   // First sort the decks so they can be compared directly
   qsort((void*)arr1, deckSize, sizeof(int), compare2);
   qsort((void*)arr2, deckSize, sizeof(int), compare2);

   return compDecks1(arr1, arr2, deckSize);
}

// This function compares 2 game states with the exception of one of the 
// player's decks. It returns 1 if the states are the same, 0 if different.
int compStates1(struct gameState s1, struct gameState s2, int excludedPlayer) {
   int i, res = 1;

   if((s1.numPlayers) != (s2.numPlayers)) {
      printf("The shuffle changed the number of players in the game.\n");
      res = 0;
   }
   for(i = 0; i <= treasure_map; i++) {
      if((s1.supplyCount[i]) != (s2.supplyCount[i])) {
         printf("The shuffle changed the number of cards in"); 
         printf(" the supply deck.\n");
         res = 0;
      }
      if((s1.embargoTokens[i]) != (s2.embargoTokens[i])) {
         printf("The shuffle changed the number of embargo tokens.\n");
         res = 0;
      }
   }
   if((s1.outpostPlayed) != (s2.outpostPlayed)) {
      printf("The shuffle changed the count of outposts played.\n");
      res = 0;
   }
   if((s1.whoseTurn) != (s2.whoseTurn)) {
      printf("The shuffle changed whose turn it is.\n");
      res = 0;
   }
   if((s1.phase) != (s2.phase)) {
      printf("The shuffle changed the phase variable.\n");
      res = 0;
   }
   if((s1.numActions) != (s2.numActions)) {
      printf("The shuffle changed the number of actions counter.\n");
      res = 0;
   }
   if((s1.numBuys) != (s2.numBuys)) {
      printf("The shuffle changed the buys counter.\n");
      res = 0;
   }
   for(i = 0; i < s1.numPlayers; i++) {
      if((s1.deckCount[i]) != (s2.deckCount[i])) {
         printf("The shuffle changed the number of cards in the deck of"); 
         printf("  Player %d.\n", i);
         res = 0;
      }
      if(i != excludedPlayer) {
         if(!(compDecks1(s1.deck[i], s2.deck[i], s1.deckCount[i]))) {
            if(!(sameCards(s1.deck[i], s2.deck[i], s1.deckCount[i]))) {
               printf("The shuffle changed the types of cards in the deck");
               printf(" of Player %d.\n", i);
            }
            else {
               printf("The shuffle changed the order of cards in the deck"); 
               printf( " of Player %d.\n", i);
            }
            res = 0;
         }
      }
      if((s1.handCount[i]) != (s2.handCount[i])) {
         printf("The shuffle changed the number of cards in the hand");
         printf(" of Player %d.\n", i);
         res = 0;
      }
      if(!(compDecks1(s1.hand[i], s2.hand[i], s1.handCount[i]))) {
         if(!(sameCards(s1.hand[i], s2.hand[i], s1.handCount[i])))
         {
            printf("The shuffle changed the types of cards in the hand");
            printf(" of Player %d.\n", i);
         }
         else {
            printf("The shuffle changed the order of cards in the hand");
            printf(" of Player %d.\n", i);
         }
         res = 0;
      }
      if((s1.discardCount[i]) != (s2.discardCount[i])) {
         printf("The shuffle changed the number of cards in the discard");
         printf(" pile for Player %d.\n", i);
         res = 0;
      }
      if(!(compDecks1(s1.discard[i], s2.discard[i], s1.discardCount[i]))) {
         if(!(sameCards(s1.discard[i], s2.discard[i], s1.discardCount[i])))
         {
            printf("The shuffle changed the types of cards in the discard");
            printf(" pile for Player %d.\n", i);
         }
         else {
            printf("The shuffle changed the order of cards in the discard");
            printf(" pile for Player %d.\n", i);
         }
         res = 0;
      }
   }
   if((s1.playedCardCount) != (s2.playedCardCount)) {
      printf("The shuffle changed the played card counter.\n");
      res = 0;
   }
   if(!(compDecks1(s1.playedCards, s2.playedCards, s1.playedCardCount))) {
      if(!(sameCards(s1.playedCards, s2.playedCards, s1.playedCardCount)))
      {
         printf("The shuffle changed the types of cards in the played card");
         printf(" pile.");
      }
      else {
         printf("The shuffle changed the order of the cards in the played");
         printf(" card pile.");
      }
      res = 0;
   }
   return res;
}

void resetStates1(struct gameState *state1, struct gameState *state2, 
                 int numPlayer, int* k, int seed) {
   // First, clear the states
   memset(state1, 0, sizeof(struct gameState));
   memset(state2, 0, sizeof(struct gameState));

   // Now, initialize one of the states and then copy it
   initializeGame(numPlayer, k, seed, state1);
   memcpy(state2, state1, sizeof(struct gameState));
}

void testShuffle() {
   int p = TEST_PLAYER, numPlayer = TEST_NUM_PLAYERS, res, testRes;
   struct gameState state, copyState;
   int seed = 123456;

   // Create several decks with varying characteristics
   int oneCardDeck[] = {village};
   int sameCardDeck[] = {silver, silver, silver, silver, 
                         silver, silver, silver, silver};
   int smallMixedDeck[] = {estate, duchy, cutpurse, smithy, steward};
   int medMixedDeck[] = {salvager, sea_hag, remodel, silver, ambassador,
                         gold, steward, feast, outpost, minion};
   int largeMixedDeck[] = {adventurer, copper, mine, treasure_map, salvager,
                           baron, great_hall, tribute, embargo, estate,
                           province, feast, village, silver, cutpurse,
                           smithy, remodel, ambassador, remodel, outpost};

   // Create an array of kingdom cards
   int k[] = {adventurer, council_room, feast, gardens, mine,
              remodel, smithy, village, baron, great_hall};

   resetStates1(&state, &copyState, numPlayer, k, seed);

   printf("\n***************Tests for shuffle() function***************\n");
   printf("**Test Configuration: ");
   printf("#Players = %d, Player Shuffling = %d**\n", numPlayer, p);

   // Test shuffling a zero-card deck. It should return an error.
   state.deckCount[p] = 0;
   res = shuffle(p, &state);
   if(res == -1) {
      printf("Correct result for deck with zero cards.\n");
   }
   else {
      printf("###Incorrect result for deck with zero cards.\n");
   }

   resetStates1(&state, &copyState, numPlayer, k, seed);

   // Test shuffling a one-card deck
   testRes = 1;
   state.deckCount[p] = copyState.deckCount[p] = 1;
   memcpy(state.deck[p], oneCardDeck, sizeof(oneCardDeck));
   memcpy(copyState.deck[p], oneCardDeck, sizeof(oneCardDeck));
   res = shuffle(p, &state);
   if(res != 0) {
      printf("###Incorrect result for deck with one card.\n");
   }
   else {
      if(!(compDecks1(state.deck[p], copyState.deck[p], state.deckCount[p])))
      {
         testRes = 0;
         printf("###Incorrect result for deck with one card. Card was");
         printf(" changed in shuffle.\n");
      }
      if(!(compStates1(state, copyState, p))) {
         testRes = 0;
         printf("###Incorrect result for deck with one card. See description");
         printf(" above.\n");
      }
   }
   if(testRes == 1) {
      printf("Correct result for deck with one card.\n");
   }

   resetStates1(&state, &copyState, numPlayer, k, seed);

   // Test shuffling a deck with all of the same cards
   testRes = 1;
   state.deckCount[p] = sizeof(sameCardDeck)/sizeof(int);
   copyState.deckCount[p] = sizeof(sameCardDeck)/sizeof(int);
   memcpy(state.deck[p], sameCardDeck, sizeof(sameCardDeck));
   memcpy(copyState.deck[p], sameCardDeck, sizeof(sameCardDeck));
   res = shuffle(p, &state);
   if(res != 0) {
      printf("###Incorrect result for deck with all of the same cards.\n");
   }
   else {
      if(!(compDecks1(state.deck[p], copyState.deck[p], state.deckCount[p])))
      {
         testRes = 0;
         printf("###Incorrect result for deck with all of the same cards."); 
         printf(" A card was changed in shuffle.\n");
      }
      if(!(compStates1(state, copyState, p))) {
         testRes = 0;
         printf("###Incorrect result for deck with all of the same cards."); 
         printf(" See description above.\n");
      }
   }
   if(testRes == 1) {
      printf("Correct result for deck with all of the same cards.\n");
   }

   resetStates1(&state, &copyState, numPlayer, k, seed);

   // Test shuffling a small deck with mixed cards
   testRes = 1;
   state.deckCount[p] = sizeof(smallMixedDeck)/sizeof(int);
   copyState.deckCount[p] = sizeof(smallMixedDeck)/sizeof(int);
   memcpy(state.deck[p], smallMixedDeck, sizeof(smallMixedDeck));
   memcpy(copyState.deck[p], smallMixedDeck, sizeof(smallMixedDeck));
   res = shuffle(p, &state);
   if(res != 0) {
      printf("###Incorrect result for 5-card deck of mixed cards.\n");
   }
   else {
      if(compDecks1(state.deck[p], copyState.deck[p], state.deckCount[p]))
      {
         testRes = 0;
         printf("###Incorrect result for 5-card deck with mixed cards."); 
         printf(" The card order did not change in the shuffle.\n");
      }
      if(!(sameCards(state.deck[p], copyState.deck[p], state.deckCount[p])))
      {
         testRes = 0;
         printf("###Incorrect result for 5-card deck with mixed cards.");
         printf(" The types of cards in the deck changed in the shuffle.\n");
      }
      if(!(compStates1(state, copyState, p))) {
         testRes = 0;
         printf("###Incorrect result for 5-card deck with mixed cards."); 
         printf(" See description above.\n");
      }
   }
   if(testRes == 1) {
      printf("Correct result for 5-card deck with mixed cards.\n");
   }

   resetStates1(&state, &copyState, numPlayer, k, seed);

   // Test shuffling a medium-sized deck with mixed cards
   testRes = 1;
   state.deckCount[p] = sizeof(medMixedDeck)/sizeof(int);
   copyState.deckCount[p] = sizeof(medMixedDeck)/sizeof(int);
   memcpy(state.deck[p], medMixedDeck, sizeof(medMixedDeck));
   memcpy(copyState.deck[p], medMixedDeck, sizeof(medMixedDeck));
   res = shuffle(p, &state);
   if(res != 0) {
      printf("###Incorrect result for 10-card deck of mixed cards.\n");
   }
   else {
      if(compDecks1(state.deck[p], copyState.deck[p], state.deckCount[p]))
      {
         testRes = 0;
         printf("###Incorrect result for 10-card deck with mixed cards."); 
         printf(" The card order did not change in the shuffle.\n");
      }
      if(!(sameCards(state.deck[p], copyState.deck[p], state.deckCount[p])))
      {
         testRes = 0;
         printf("###Incorrect result for 10-card deck with mixed cards.");
         printf(" The types of cards in the deck changed in the shuffle.\n");
      }
      if(!(compStates1(state, copyState, p))) {
         testRes = 0;
         printf("###Incorrect result for 10-card deck with mixed cards."); 
         printf(" See description above.\n");
      }
   }
   if(testRes == 1) {
      printf("Correct result for 10-card deck with mixed cards.\n");
   }

   resetStates1(&state, &copyState, numPlayer, k, seed);

   // Test shuffling a large deck with mixed cards
   testRes = 1;
   state.deckCount[p] = sizeof(largeMixedDeck)/sizeof(int);
   copyState.deckCount[p] = sizeof(largeMixedDeck)/sizeof(int);
   memcpy(state.deck[p], largeMixedDeck, sizeof(largeMixedDeck));
   memcpy(copyState.deck[p], largeMixedDeck, sizeof(largeMixedDeck));
   res = shuffle(p, &state);
   if(res != 0) {
      printf("###Incorrect result for 20-card deck of mixed cards.\n");
   }
   else {
      if(compDecks1(state.deck[p], copyState.deck[p], state.deckCount[p]))
      {
         testRes = 0;
         printf("###Incorrect result for 20-card deck with mixed cards."); 
         printf(" The card order did not change in the shuffle.\n");
      }
      if(!(sameCards(state.deck[p], copyState.deck[p], state.deckCount[p])))
      {
         testRes = 0;
         printf("###Incorrect result for 20-card deck with mixed cards.");
         printf(" The types of cards in the deck changed in the shuffle.\n");
      }
      if(!(compStates1(state, copyState, p))) {
         testRes = 0;
         printf("###Incorrect result for 20-card deck with mixed cards."); 
         printf(" See description above.\n");
      }
   }
   if(testRes == 1) {
      printf("Correct result for 20-card deck with mixed cards.\n");
   }

   // Test to see if consecutive shuffles produce different results
   memcpy(copyState.deck[p], state.deck[p], sizeof(largeMixedDeck));
   shuffle(p, &state);
   if(compDecks1(state.deck[p], copyState.deck[p], state.deckCount[p]))
   {
      testRes = 0;
      printf("###Incorrect result for consecutive shuffles."); 
      printf(" The card order did not change in the second shuffle.\n");
   }
   if(testRes == 1) {
      printf("Correct result for consecutive shuffles.\n");
   }

   resetStates1(&state, &copyState, numPlayer, k, seed);

   // Test sending invalid players to shuffle()
   // First, choose player greater than the maximum player
   p = 4;
   printf("**Test Configuration: ");
   printf("#Players = %d, Player Shuffling = %d**\n", numPlayer, p);
   res = shuffle(p, &state);
   if(res == -1) {
      printf("Correct result for sending player greater than max player.\n");
   }
   else {
      printf("###Incorrect result for sending player greater than max ");
      printf("player.\n");
   }
   // Next, choose player that is a negative number
   p = -1;
   printf("**Test Configuration: ");
   printf("#Players = %d, Player Shuffling = %d**\n", numPlayer, p);
   res = shuffle(p, &state);
   if(res == -1) {
      printf("Correct result for sending player that is negative number.\n");
   }
   else {
      printf("###Incorrect result for sending player that is a negative ");
      printf("number.\n");
   }
}

int main(int argc, char *argv[]) {
   testShuffle();
   return 0;
}
