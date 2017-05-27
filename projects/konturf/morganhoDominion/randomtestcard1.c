/******************************************************************************
 * Filename:     cardtest3.c
 * Author:       Fred Kontur
 * Date Written: May 3, 2017
 * Last Edited:  May 14, 2017
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

#define NUM_ITER 2000
#define MAX_HAND_COUNT 10
#define MAX_DECK_COUNT 10
#define MAX_DISCARD_COUNT 10
#define MAX_PLAYED_COUNT 10

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
   int i, res = 1, numPlayer = s1.numPlayers;

   // If numPlayer is greater than MAX_PLAYERS, then change it to MAX_PLAYERS
   // so the array tests don't go out of bounds
   if(numPlayer > MAX_PLAYERS) {
      numPlayer = MAX_PLAYERS;
   } 

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
   for(i = 0; i < numPlayer; i++) {
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
      printf(" pile.\n");
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

int randomTestGenerator(struct gameState *state1, struct gameState *state2,
                        int *choice)
{
   int i, j, currPlayer, numPlayer, numHand, handPos, numDeck, numDiscard;
   int numPlayed;

   // First, clear the states
   memset(state1, 0, sizeof(struct gameState));
   memset(state2, 0, sizeof(struct gameState));

   // Generate a random number of players between 1 and 5
   numPlayer = (rand() % 5) + 1;
   state1->numPlayers = numPlayer;

   // Choose a random current player between 0 and 3
   currPlayer = rand() % 4;
   state1->whoseTurn = currPlayer;

   // Choose a random number of cards for each of the sets of cards in supply
   // between -5 and 100 and similarly for embargo tokens
   for(i = 0; i <= treasure_map; i++) {
      state1->supplyCount[i] = (rand() % 106) - 5;
      state1->embargoTokens[i] = (rand() % 106) - 5;
   }

   // Choose a random number for outpostPlayed between 0 and 10
   state1->outpostPlayed = rand() % 11;

   // Choose a random number for outpostTurn between 0 and 10
   state1->outpostTurn = rand() % 11;

   // Choose a random number for phase between 0 and 4
   state1->phase = rand() % 5;

   // Choose a random number for number of actions between 0 and 5
   state1->numActions = rand() % 6;

   // Choose a random number of coins between -1 and 20
   state1->coins = (rand() % 22) - 1;

   // Choose a random number of buys between -1 and 5
   state1->numBuys = (rand() % 7) - 1; 

   // Randomly select the card that the player will attempt to gain with the
   // Feast card
   *choice = rand() % (treasure_map + 1);
   
   // Choose a random number of cards in the current player's hand between 1 
   // and MAX_HAND_COUNT
   numHand = (rand() % MAX_HAND_COUNT) + 1;
   state1->handCount[currPlayer] = numHand;

   // Put random cards in the hand of the current player between -1 and 
   // treasure_map + 1. One of the cards will be replaced by the Feast card
   for(i = 0; i < numHand; i++) {
      state1->hand[currPlayer][i] = (rand() % (treasure_map + 3)) - 1;
   }

   // Choose a random hand position for the Feast card
   handPos = rand() % numHand;
   state1->hand[currPlayer][handPos] = feast;

   // If numPlayer is greater than MAX_PLAYERS, then make it equal to 
   // MAX_PLAYERS so that the arrays don't go out of bounds for the following
   // tasks
   if(numPlayer > MAX_PLAYERS) {
      numPlayer = MAX_PLAYERS;
   }

   // For the other players, choose a random number of cards for their hands
   // between - and MAX_HAND_COUNT and populate the hands with random cards
   // with IDs between -1 and treasure_map + 1
   for(i = 0; i < numPlayer; i++) {
      numHand = rand() % (MAX_HAND_COUNT + 1);
      if(i != currPlayer) {
         state1->handCount[i] = numHand;
         for(j = 0; j < numHand; j++) {
            state1->hand[i][j] = (rand() % (treasure_map + 3)) - 1;
         }
      }
   }

   // Choose a random number of cards for all of the players' decks between 0 
   // and MAX_DECK_COUNT
   for(i = 0; i < numPlayer; i++) {
      numDeck = rand() % (MAX_DECK_COUNT + 1);
      state1->deckCount[i] = numDeck;
      // Put random cards in the deck with IDs between -1 and treasure_map + 1
      for(j = 0; j < numDeck; j++) {
         state1->deck[i][j] = (rand() % (treasure_map + 3)) - 1;
      }
   }

   // Choose a random number of cards for all of the players' discard piles 
   // between 0 and MAX_DISCARD_COUNT
   for(i = 0; i < numPlayer; i++) {
      numDiscard = rand() % (MAX_DISCARD_COUNT + 1);
      state1->discardCount[i] = numDiscard;
      // Put random cards in the discard pile with IDs between -1 and 
      // treasure_map + 1
      for(j = 0; j < numDiscard; j++) {
         state1->discard[i][j] = (rand() % (treasure_map + 3)) - 1;
      }
   }

   // Choose a random number of cards for the played card pile between 0 and
   // MAX_PLAYED_COUNT and put random cards in the played card pile with IDs
   // between -1 and treasure_map + 1
   numPlayed = rand() % (MAX_PLAYED_COUNT + 1);
   state1->playedCardCount = numPlayed;
   for(i = 0; i < numPlayed; i++) {
      state1->playedCards[i] = (rand() % (treasure_map + 3)) - 1;
   }

   // Copy the state
   memcpy(state2, state1, sizeof(struct gameState));

   return handPos;
}

void testFeast() {
   int i, j, handPos, res, res1, res2;
   int numPlayer, currPlayer, numHand, gainedCard;
   struct gameState state, copyState;

   srand(time(NULL));

   printf("***************Tests for Feast Card***************\n");

   for(i = 0; i < NUM_ITER; i++) {
      handPos = randomTestGenerator(&state, &copyState, &gainedCard);
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
            printf("\nAttempted card to be gained: %d\n", gainedCard);
            printf("Incorrect result, card not in supply but was allowed ");
            printf("to be gained\n\n");
         }
      }
      else if(copyState.phase != 0) {
         if(res != -1) {
            printf("Phase: %d\n", copyState.phase);
            printf("Incorrect result, Feast was played when phase ");
            printf("was not 0\n\n");
         }
      }
      else if(copyState.numActions < 1) {
         if(res != -1) {
            printf("Number of Actions: %d\n", copyState.numActions);
            printf("Incorrect result, Feast was played when numActions ");
            printf("was less than 1\n\n");
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
            printf("\nIncorrect result, playCard() returned -1\n\n");
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
               printf("\nIncorrect result, see description above\n\n");
            }
         }
      }
   }
}

int main(int argc, char *argv[]) {
   testFeast();
   return 0;
}
