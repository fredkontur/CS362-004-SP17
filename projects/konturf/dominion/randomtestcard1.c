/******************************************************************************
 * Filename:     cardtest3.c
 * Author:       Fred Kontur
 * Date Written: May 3, 2017
 * Last Edited:  May 6, 2017
 * Description:  This file contains a random test generator for the Feast card
 *               in dominion.c
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

// This function compares 2 game states with the exception of the supply cards, 
// numActions, and the hand and discard pile of the current player. It returns 
// 1 if the states are the same, 0 if different.
int compStates(struct gameState s1, struct gameState s2, int currPlayer) {
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
      if(!(compDecks(s1.deck[i], s2.deck[i], s1.deckCount[i]))) {
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
         if(!(compDecks(s1.hand[i], s2.hand[i], s1.handCount[i]))) {
            printf("Feast changed the cards in the hand");
            printf(" of Player %d.\n", i);
            res = 0;
         }
         if((s1.discardCount[i]) != (s2.discardCount[i])) {
            printf("Feast changed the number of cards in the discard");
            printf(" pile for Player %d.\n", i);
            res = 0;
         }
         if(!(compDecks(s1.discard[i], s2.discard[i], s1.discardCount[i]))) {
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
   if(!(compDecks(s1.playedCards, s2.playedCards, s1.playedCardCount))) {
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
                         int *k, int seed, int *choice)
{
   int i, j, currPlayer, numPlayer, numHand, handPos, numDeck, numDiscard;
   int currCard;
   int kChosen[treasure_map + 1], possChoices[adventurer + 10];

   // Start to populate the possChoices[] array
   for(i = 0; i < adventurer; i++) {
      possChoices[i] = i;
   }

   // Create a kingdom card deck
   // Keep track of which kingdom cards were chosen by initializing
   // kChosen with zeroes (0 = not chosen, 1 = chosen)
   memset(kChosen, 0, sizeof(kChosen));
   // Randomly choose cards for the kingdom card deck
   for(i = 0; i < 10; i++) {
      do {
      currCard = (rand() % (treasure_map - gold)) + adventurer;
      } while(kChosen[currCard]);
      kChosen[currCard] = 1;
      possChoices[i + adventurer] = k[i] = currCard;
   }

   // Randomly select the card that the player will attempt to gain with the
   // Feast card
   i = rand() % (sizeof(possChoices) / sizeof(int));
   *choice = possChoices[i];

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

   // Put random cards in the current player's hand. One of the cards will be
   // replaced by the Feast card
   for(i = 0; i < numHand; i++) {
      state1->hand[currPlayer][i] = rand() % (treasure_map + 1);
   }

   // Choose a random hand position for the Feast card
   handPos = rand() % numHand;
   state1->hand[currPlayer][handPos] = feast;

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

   // Randomize the number of each kingdom card that is available
   for(i = 0; i < 10; i++) {
      state1->supplyCount[k[i]] = rand() % 11;
   }

   // Copy the state
   memcpy(state2, state1, sizeof(struct gameState));

   return handPos;
}

void testFeast() {
   int i, j, seed, handPos, res, res1, res2;
   int numPlayer, currPlayer, numHand, gainedCard;
   int k[10];
   struct gameState state, copyState;

   srand(time(NULL));

   printf("***************Tests for Feast Card***************\n");

   for(i = 0; i < NUM_ITER; i++) {
      seed = rand();
      handPos = randomTestGenerator(&state, &copyState, k, seed, &gainedCard);
      numPlayer = state.numPlayers;
      currPlayer = state.whoseTurn;
      numHand = state.handCount[currPlayer];

      // Play the Feast card
      res = playCard(handPos, gainedCard, 0, 0, &state);

      if(getCost(gainedCard) > 5) {
         if(res != -1) {
            printf("Test Conditions\n");
            printf("# Players: %d, ", numPlayer);
            printf("Current Player: %d\n", currPlayer);
            printf("Hand Cards:");
            for(j = 0; j < numHand; j++) {
               printf(" %d", copyState.hand[currPlayer][j]);
            }
            printf("\nKingdom Cards (Count):");
            for(j = 0; j < 10; j++) {
               printf(" %d(%d)", k[j], copyState.supplyCount[k[j]]);
            }
            printf("\nAttempted card to be gained: %d\n", gainedCard);
            printf("Incorrect result, card costing more than 5 was ");
            printf("gained\n\n");
         }
      }
      else if(copyState.supplyCount[gainedCard] <= 0) {
         if(res != -1) {
            printf("Test Conditions\n");
            printf("# Players: %d, ", numPlayer);
            printf("Current Player: %d\n", currPlayer);
            printf("Hand Cards:");
            for(j = 0; j < numHand; j++) {
               printf(" %d", copyState.hand[currPlayer][j]);
            }
            printf("\nKingdom Cards (Count):");
            for(j = 0; j < 10; j++) {
               printf(" %d(%d)", k[j], copyState.supplyCount[k[j]]);
            }
            printf("\nAttempted card to be gained: %d\n", gainedCard);
            printf("Incorrect result, card not in supply but was allowed ");
            printf("to be gained\n\n");
         }
      }
      else {
         if(res == -1) {
            printf("Test Conditions\n");
            printf("# Players: %d, ", numPlayer);
            printf("Current Player: %d\n", currPlayer);
            printf("Hand Cards:");
            for(j = 0; j < numHand; j++) {
               printf(" %d", copyState.hand[currPlayer][j]);
            }
            printf("\nKingdom Cards (Count):");
            for(j = 0; j < 10; j++) {
               printf(" %d(%d)", k[j], copyState.supplyCount[k[j]]);
            }
            printf("Incorrect result, playCard() returned -1\n\n");
         }
         else {
            // Check the resulting state for errors
            res1 = compStates(copyState, state, currPlayer);
            res2 = checkFeastChanges(copyState, state, currPlayer);
            if((res1 == 0) || (res2 == 0)) {
               printf("Test Conditions\n");
               printf("# Players: %d, ", numPlayer);
               printf("Current Player: %d\n", currPlayer);
               printf("Hand Cards:");
               for(j = 0; j < numHand; j++) {
                  printf(" %d", copyState.hand[currPlayer][j]);
               }
               printf("\nKingdom Cards (Count):");
               for(j = 0; j < 10; j++) {
                  printf(" %d(%d)", k[j], copyState.supplyCount[k[j]]);
               }
               printf("Incorrect result, see description above\n\n");
            }
         }
      }
   }
}

int main(int argc, char *argv[]) {
   testFeast();
   return 0;
}
