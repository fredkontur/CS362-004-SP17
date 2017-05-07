/******************************************************************************
 * Filename:     randomtestadventrer.c
 * Author:       Fred Kontur
 * Date Written: May 1, 2017
 * Last Edited:  May 6, 2017
 * Description:  This file contains a random test generator for the Adventurer 
 *               card in dominion.c
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
#include <time.h>

#define NUM_ITER 200
#define MAX_HAND_COUNT 10
#define MAX_DECK_COUNT 10
#define MAX_DISCARD_COUNT 10

// Returns 1 if decks are the same, 0 if different
int compDecks(int* arr1, int* arr2, int deckSize) {
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
int compStates(struct gameState s1, struct gameState s2, int currPlayer) {
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
            printf(" Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks(s1.deck[i], s2.deck[i], s1.deckCount[i]))) {
            printf("Adventurer changed the cards in the deck");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if((s1.handCount[i]) != (s2.handCount[i])) {
            printf("Adventurer changed the number of cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks(s1.hand[i], s2.hand[i], s1.handCount[i]))) {
            printf("Adventurer changed the cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if((s1.discardCount[i]) != (s2.discardCount[i])) {
            printf("Adventurer changed the number of cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks(s1.discard[i], s2.discard[i], s1.discardCount[i]))) {
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
      if((orgnlHndCnt + 2) != fnlHndCnt) {
         printf("Current player did not pick up 2 Treasures ");
         printf("and discard non-Treasure cards as required\n");
         res = 0;
      }
      else {
         for(j = 0; j < 2; j++) {
            currCard = finalState.hand[currPlayer][fnlHndCnt + i];
            if((currCard != copper) || (currCard != silver) || 
               (currCard != gold))
            {
               printf("Current player did not pick up 2 Treasures ");
               printf("and discard non-Treasure cards as required\n");
               res = 0;
            }
         }
      }
   }
   else {
      if(fnlTCnt != 0) {
         printf("Treasures were not removed from deck and discard ");
         printf("piles as should have occurred.\n");
         res = 0;
      }
      if((orgnlHndCnt + orgnlTCnt) != fnlHndCnt) {
         printf("Current player did not pick up %d Treasure ", orgnlTCnt);
         printf("and discard non-Treasure cards as required\n");
         res = 0;
      }
      else {
         for(j = 0; j < orgnlTCnt; j++) {
            currCard = finalState.hand[currPlayer][fnlHndCnt + i];
            if((currCard != copper) || (currCard != silver) || 
               (currCard != gold))
            {
               printf("Current player did not pick up %d Treasure", orgnlTCnt);
               printf(" and discard non-Treasure cards as required\n");
               res = 0;
            }
         }
      }
   }
   // Check that the correct number of non-Treasure cards were moved from 
   // deck/discard to the discard pile
   diff =  (orgnlDckCnt + orgnlDscrdCnt + orgnlTCnt) - 
           (fnlDckCnt + fnlDscrdCnt + fnlTCnt); 
   if(diff != (fnlDscrdCnt - orgnlDscrdCnt)) {
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

void resetStates(struct gameState *state1, struct gameState *state2, 
                 int numPlayer, int* k, int seed) {
   // First, clear the states
   memset(state1, 0, sizeof(struct gameState));
   memset(state2, 0, sizeof(struct gameState));

   // Now, initialize one of the states and then copy it
   initializeGame(numPlayer, k, seed, state1);
   memcpy(state2, state1, sizeof(struct gameState));
}

int randomTestGenerator(struct gameState *state1, struct gameState *state2,
                         int *k, int seed)
{
   int i, j, currPlayer, numPlayer, numHand, handPos, numDeck, numDiscard;
   int currCard, kChosen[treasure_map + 1];

   // Create a kingdom card deck
   // Keep track of which kingdom cards were chosen by initializing kChosen
   // with zeroes (0 = not chosen, 1 = chosen)
   memset(kChosen, 0, sizeof(kChosen));
   // Randomly choose cards for the kingdom card deck
   for(i = 0; i < 10; i++) {
      do {
         currCard = (rand() % (treasure_map - gold)) + adventurer;
      } while(kChosen[currCard]);
      kChosen[currCard] = 1;
      k[i] = currCard;
   }

   // Generate a random number of players between 2 and 4
   numPlayer = (rand() % 3) + 2;

   // Initialize a new game state and its copy
   resetStates(state1, state2, numPlayer, k, seed);

   // Choose a random current player between 0 and numPlayer - 1
   currPlayer = rand() % numPlayer;
   state1->whoseTurn = currPlayer;

   // Choose a random number of cards in the current player's hand between 1 
   // and MAX_HAND_COUNT
   numHand = (rand() % MAX_HAND_COUNT) + 1;
   state1->handCount[currPlayer] = numHand;

   // Put random cards in the hand of the current player. One of the cards will 
   // be replaced by the Adventurer card
   for(i = 0; i < numHand; i++) {
      state1->hand[currPlayer][i] = rand() % (treasure_map + 1);
   }

   // Choose a random hand position for the Adventurer card
   handPos = rand() % numHand;
   state1->hand[currPlayer][handPos] = adventurer;

   // The other players should have 5 cards in their hands. Choose random
   // cards for their hands
   for(i = 0; i < numPlayer; i++) {
      if(i != currPlayer) {
         for(j = 0; j < 5; j++) {
            state1->hand[i][j] = rand() % (treasure_map + 1);
         }
      }
   }

   // Choose a random number of cards for all of the players' decks between 0 
   // and MAX_DECK_COUNT
   for(i = 0; i < numPlayer; i++) {
      numDeck = rand() % (MAX_DECK_COUNT + 1);
      state1->deckCount[i] = numDeck;
      // Put random cards in the deck
      for(j = 0; j < numDeck; j++) {
         state1->deck[i][j] = rand() % (treasure_map + 1);
      }
   }

   // Choose a random number of cards for all of the players' discard piles 
   // between 0 and MAX_DISCARD_COUNT
   for(i = 0; i < numPlayer; i++) {
      numDiscard = rand() % (MAX_DISCARD_COUNT + 1);
      state1->discardCount[i] = numDiscard;
      // Put random cards in the discard pile
      for(j = 0; j < numDiscard; j++) {
         state1->discard[i][j] = rand() % (treasure_map + 1);
      }
   }

   // Copy the state
   memcpy(state2, state1, sizeof(struct gameState));

   return handPos;
}

void testAdventurer() {
   int i, j, seed, handPos, res, res1, res2;
   int numPlayer, currPlayer, numHand, numDeck, numDiscard;
   int k[10];
   struct gameState state, copyState;

   srand(time(NULL));

   printf("***************Tests for Adventurer Card***************\n");

   for(i = 0; i < NUM_ITER; i++) {
      seed = rand();
      handPos = randomTestGenerator(&state, &copyState, k, seed);
      numPlayer = state.numPlayers;
      currPlayer = state.whoseTurn;
      numHand = state.handCount[currPlayer];
      numDeck = state.deckCount[currPlayer];
      numDiscard = state.discardCount[currPlayer];

      // Play the Adventurer card
      res = playCard(handPos, 0, 0, 0, &state);

      if(res == -1) {
         printf("Test Conditions\n");
         printf("# Players: %d, Current Player: %d\n", numPlayer, currPlayer);
         printf("Hand Cards:");
         for(j = 0; j < numHand; j++) {
            printf(" %d", copyState.hand[currPlayer][j]);
         }
         printf("\nDeck Cards:");
         for(j = 0; j < numDeck; j++) {
            printf(" %d", copyState.deck[currPlayer][j]);
         }
         printf("\nDiscard Pile:");
         for(j = 0; j < numDiscard; j++) {
            printf(" %d", copyState.discard[currPlayer][j]);
         }
         printf("\n");
         printf("###Incorrect result, playCard() returned -1\n\n");
      }
      else {
         // Check the resulting state for errors
         res1 = compStates(copyState, state, currPlayer);
         res2 = checkAdventurerChanges(copyState, state, currPlayer);
         if((res1 == 0) || (res2 == 0)) {
            printf("Test Conditions\n");
            printf("# Players: %d, ", numPlayer);
            printf("Current Player: %d\n", currPlayer);
            printf("Hand Cards:");
            for(j = 0; j < numHand; j++) {
               printf(" %d", copyState.hand[currPlayer][j]);
            }
            printf("\nDeck Cards:");
            for(j = 0; j < numDeck; j++) {
               printf(" %d", copyState.deck[currPlayer][j]);
            }
            printf("\nDiscard Pile:");
            for(j = 0; j < numDiscard; j++) {
               printf(" %d", copyState.discard[currPlayer][j]);
            }
            printf("\n");
            printf("###Incorrect result, see description above\n\n");
         }
      }
   }
}

int main(int argc, char *argv[]) {
   testAdventurer();
   return 0;
}
