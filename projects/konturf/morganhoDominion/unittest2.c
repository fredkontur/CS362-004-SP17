/******************************************************************************
 * Filename:     unittest2.c
 * Author:       Fred Kontur
 * Date Written: April 22, 2016
 * Last Edited:  April 22, 2016
 * Description:  This file contains a unit test for the initializeGame() 
 *               function in dominion.c
 * Business Requirements for initializeGame():
 * 1. initializeGame() should set up a game state for the start of the game
 *    according to the standard Dominion rules. This includes
 *    a) Supply with the proper number of Treasure, Victory, Curse, and Kingdom
 *       cards based on the number of players in the game.
 *    b) Each player has 10 cards - 7 coppers and 3 estates. 5 of the cards
 *       are in each of the players' hands and 5 are in each of the players'
 *       decks.
 * 2. initializeGame() should check that valid game variables have been passed
 *    to it, including a valid number of players and a valid set of kingdom
 *    cards.
******************************************************************************/
#include "dominion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare3(const void* a, const void* b) {
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

// Returns 1 if the game is in a valid starting state, 0 if not
int checkGameStart(struct gameState state, int *k) {
   int i, j = 0, res = 1, numVict, numCopper, numEstate; 
   int numPlayer = state.numPlayers;

   numVict = ((numPlayer == 2) ? 8 : 12);

   // Sort the kingdom cards to make them easier to compare to the supply
   qsort((void*)k, 10, sizeof(int), compare3);

   if((numPlayer < 2) || (numPlayer > 4)) {
      printf("Invalid number of players: %d\n", numPlayer);
      return 0;
   }

   // Check to make sure the supply has the right number and
   // types of cards
   if(state.supplyCount[curse] != ((10 * numPlayer) - 10)) {
      printf("Invalid number of curse cards ");
      printf("in supply: %d\n", state.supplyCount[curse]);
      res = 0; 
   }
   if(state.supplyCount[copper] != ((60 - (7 * numPlayer)))) {
      printf("Invalid number of copper cards ");
      printf("in supply: %d\n", state.supplyCount[copper]);
      res = 0;
   }
   if(state.supplyCount[silver] != 40) {
      printf("Invalid number of silver cards ");
      printf("in supply: %d\n", state.supplyCount[silver]);
      res = 0;
   }
   if(state.supplyCount[gold] != 30) {
      printf("Invalid number of gold cards ");
      printf("in supply: %d\n", state.supplyCount[gold]);
      res = 0;
   }
   if(state.supplyCount[estate] != numVict) {
      printf("Invalid number of estate cards ");
      printf("in supply: %d\n", state.supplyCount[estate]);
      res = 0;
   }
   if(state.supplyCount[duchy] != numVict) {
      printf("Invalid number of duchy cards ");
      printf("in supply: %d\n", state.supplyCount[duchy]);
      res = 0;
   }
   if(state.supplyCount[province] != numVict) {
      printf("Invalid number of province cards ");
      printf("in supply: %d\n", state.supplyCount[province]);
      res = 0;
   }
   for(i = adventurer; i <= treasure_map; i++) {
      if((j < 10) && (k[j] == i)) {
         if((i == gardens) || (i == great_hall)) {
            if(state.supplyCount[i] != numVict) {
               printf("Invalid number of card #%d ", i);
               printf("in supply: %d\n", state.supplyCount[i]);
               res = 0;
            }
         }
         else {
            if(state.supplyCount[i] != 10) {
               printf("Invalid number of card #%d ", i);
               printf("in supply: %d\n", state.supplyCount[i]);
               res = 0;
            }
         }
         j++;
      }
      else {
         if(state.supplyCount[i] > 0) {
            printf("Card #%d is in supply but should not be\n", i);
            res = 0;
         }
      }
   }

   // Check the players' hands and decks to make sure they have the
   // right number and types of cards
   for(i = 0; i < numPlayer; i++) {
      numCopper = numEstate = 0;
      if(state.handCount[i] != 5) {
         printf("Every player should have 5 cards in their hand ");
         printf("at the start of the game\n");
         printf("The hand of Player %d has %d cards ", i, state.handCount[i]);
         printf("rather than 5\n");
         res = 0;
      }
      if(state.deckCount[i] != 5) {
         printf("Every player should have 5 cards in their deck ");
         printf("at the start of the game\n");
         printf("The deck of Player %d has %d cards ", i, state.handCount[i]);
         printf("rather than 5\n");
         res = 0;
      }
      for(j = 0; j < state.handCount[i]; j++) {
         if(state.hand[i][j] == copper) {
            numCopper++;
         }
         else if(state.hand[i][j] == estate) {
            numEstate++;
         } 
      }
      for(j = 0; j < state.deckCount[i]; j++) {
         if(state.deck[i][j] == copper) {
            numCopper++;
         }
         else if(state.deck[i][j] == estate) {
            numEstate++;
         }
      }
      if(numCopper != 7) {
         printf("Player %d has %d copper cards rather than 7\n", i, numCopper);
         res = 0;
      }
      if(numEstate != 3) {
         printf("Player %d has %d estate cards rather than 3\n", i, numEstate);
         res = 0;
      }
   }

   // Make sure first player's turn is set up
   if(state.whoseTurn != 0) {
      printf("Invalid player starting the game: %d\n", state.whoseTurn);
      res = 0;
   }
   if(state.outpostPlayed != 0) {
      printf("Outpost played is %d rather than 0\n", state.outpostPlayed);
      res = 0;
   }
   if(state.phase != 0) {
      printf("Phase is %d rather than 0\n", state.phase);
      res = 0;
   }
   if(state.numActions != 1) {
      printf("Number of actions available for starting player is ");
      printf("%d rather than 1\n", state.numActions);
      res = 0;
   }
   if(state.numBuys != 1) {
      printf("Number of buys available for starting player is ");
      printf("%d rather than 1\n", state.numBuys);
      res = 0;
   }
   if(state.playedCardCount != 0) {
      printf("Number of played cards is %d ", state.playedCardCount);
      printf("rather than 0\n");
      res = 0;
   }

   return res;
}

void testInitializeGame() {
   int numPlayer, res, seed = 123456;
   struct gameState state;
   int kNoVictory[] = {minion, sea_hag, cutpurse, ambassador, steward,
                       adventurer, smithy, tribute, baron, salvager};
   int kVictory[] = {treasure_map, smithy, gardens, ambassador, sea_hag,
                     feast, great_hall, cutpurse, tribute, adventurer};
   int kIncorrect[] = {gardens, cutpurse, steward, remodel, mine,
                       baron, steward, adventurer, salvager, minion};

   printf("\n***********Tests for initializeGame() function*************\n");

   // Send initializeGame() too few players
   numPlayer = 1;
   res = initializeGame(numPlayer, kNoVictory, seed, &state);
   if(res == -1) {
      printf("Correct result for too few players.\n");
   }

   // Send initializeGame() too many players
   numPlayer = MAX_PLAYERS + 1;
   res = initializeGame(numPlayer, kNoVictory, seed, &state);
   if(res == -1) {
      printf("Correct result for too many players.\n");
   }

   // Test initializeGame() with duplicate kingdom cards
   numPlayer = 2;
   res = initializeGame(numPlayer, kIncorrect, seed, &state);
   if(res == -1) {
      printf("Correct result for duplicate kingdom cards.\n");
   }
   
   // Test initializeGame() with 2 players
   numPlayer = 2;
   res = initializeGame(numPlayer, kNoVictory, seed, &state);
   if(res == -1) {
      printf("###Incorrect result for 2 players and kingdom deck with ");
      printf("no victory cards.\n");
   }
   else {
      if(state.numPlayers != numPlayer) {
         printf("###Incorrect result - %d players ", state.numPlayers);
         printf("rather than %d players.\n", numPlayer);
      }
      else {
         res = checkGameStart(state, kNoVictory);
         if(res == 0) {
            printf("###Incorrect result for 2 players and kingdom deck with ");
            printf("no victory cards. See description above.\n");
         }
         else {
            printf("Correct result for 2 players and kingdom deck with ");
            printf("no victory cards.\n");
         }
      }
   }
   res = initializeGame(numPlayer, kVictory, seed, &state);
   if(res == -1) {
      printf("###Incorrect result for 2 players and kingdom deck with ");
      printf("victory cards.");
   }
   else {
      if(state.numPlayers != numPlayer) {
         printf("###Incorrect result - %d players ", state.numPlayers);
         printf("rather than %d players.\n", numPlayer);
      }
      else {
         res = checkGameStart(state, kVictory);
         if(res == 0) {
            printf("###Incorrect result for 2 players and kingdom deck with ");
            printf("victory cards. See description above.\n");
         }
         else {
            printf("Correct result for 2 players and kingdom deck with ");
            printf("no victory cards.\n");
         }
      }
   }
   // Test initializeGame() with 3 players
   numPlayer = 3;
   res = initializeGame(numPlayer, kNoVictory, seed, &state);
   if(res == -1) {
      printf("###Incorrect result for 3 players and kingdom deck with ");
      printf("no victory cards.");
   }
   else {
      if(state.numPlayers != numPlayer) {
         printf("###Incorrect result - %d players ", state.numPlayers);
         printf("rather than %d players.\n", numPlayer);
      }
      else {
         res = checkGameStart(state, kNoVictory);
         if(res == 0) {
            printf("###Incorrect result for 3 players and kingdom deck with ");
            printf("no victory cards. See description above.\n");
         }
         else {
            printf("Correct result for 3 players and kingdom deck with ");
            printf("no victory cards.\n");
         }
      }
   }
   res = initializeGame(numPlayer, kVictory, seed, &state);
   if(res == -1) {
      printf("###Incorrect result for 3 players and kingdom deck with ");
      printf("victory cards.\n");
   }
   else {
      if(state.numPlayers != numPlayer) {
         printf("###Incorrect result - %d players ", state.numPlayers);
         printf("rather than %d players.\n", numPlayer);
      }
      else {
         res = checkGameStart(state, kVictory);
         if(res == 0) {
            printf("###Incorrect result for 3 players and kingdom deck with ");
            printf("victory cards. See description above.\n");
         }
         else {
            printf("Correct result for 3 players and kingdom deck with ");
            printf("vicotry cards.\n");
         }
      }
   }
   // Test initializeGame() with 4 players
   numPlayer = 4;
   res = initializeGame(numPlayer, kNoVictory, seed, &state);
   if(res == -1) {
      printf("###Incorrect result for 4 players and kingdom deck with ");
      printf("no victory cards.");
   }
   else {
      if(state.numPlayers != numPlayer) {
         printf("###Incorrect result - %d players ", state.numPlayers);
         printf("rather than %d players.\n", numPlayer);
      }
      else {
         res = checkGameStart(state, kNoVictory);
         if(res == 0) {
            printf("###Incorrect result for 4 players and kingdom deck with ");
            printf("no victory cards. See description above.\n");
         }
         else {
            printf("Correct result for 4 players and kingdom deck with ");
            printf("no victory cards.\n");
         }
      }
   }
   res = initializeGame(numPlayer, kVictory, seed, &state);
   if(res == -1) {
      printf("###Incorrect result for 4 players and kingdom deck with ");
      printf("victory cards.\n");
   }
   else {
      if(state.numPlayers != numPlayer) {
         printf("###Incorrect result - %d players ", state.numPlayers);
         printf("rather than %d players.\n", numPlayer);
      }
      else {
         res = checkGameStart(state, kVictory);
         if(res == 0) {
            printf("###Incorrect result for 4 players and kingdom deck with ");
            printf("victory cards. See description above.\n");
         }
         else {
            printf("Correct result for 4 players and kingdom deck with ");
            printf("vicotry cards.\n");
         }
      }
   }
}

int main(int argc, char *argv[]) {
   testInitializeGame();
   return 0;
}
