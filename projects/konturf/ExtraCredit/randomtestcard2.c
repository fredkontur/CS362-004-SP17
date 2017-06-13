/******************************************************************************
 * Filename:     randomtestcard2.c
 * Author:       Fred Kontur
 * Date Written: May 6, 2017
 * Last Edited:  June 11, 2017
 * Description:  This file contains a random test generator for the Mine card 
 *               in dominion.c 
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

#define MAX_HAND_COUNT 10
#define MAX_DECK_COUNT 10
#define MAX_DISCARD_COUNT 10
#define MAX_PLAYED_COUNT 10

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
int compDecks(int* arr1, int* arr2, int deckSize) {
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
int compStates(struct gameState s1, struct gameState s2, int currPlayer) {
   int i, res = 1, numPlayer = s1.numPlayers;

   // If numPlayer is greater than MAX_PLAYERS, then change it to MAX_PLAYERS
   // so the array tests don't go out of bounds
   if(numPlayer > MAX_PLAYERS) {
      numPlayer = MAX_PLAYERS;
   }

   if((s1.numPlayers) != (s2.numPlayers)) {
      res = 0;
   }
   for(i = 0; i <= treasure_map; i++) {
      if((i != copper) && (i != silver) && (i != gold)) {
         if((s1.supplyCount[i]) != (s2.supplyCount[i])) {
            res = 0;
         }
      }
      if((s1.embargoTokens[i]) != (s2.embargoTokens[i])) {
         res = 0;
      }
   }
   if((s1.outpostPlayed) != (s2.outpostPlayed)) {
      res = 0;
   }
   if((s1.whoseTurn) != (s2.whoseTurn)) {
      res = 0;
   }
   if((s1.phase) != (s2.phase)) {
      res = 0;
   }
   if((s1.numBuys) != (s2.numBuys)) {
      res = 0;
   }
   for(i = 0; i < numPlayer; i++) {
      if((s1.deckCount[i]) != (s2.deckCount[i])) {
         res = 0;
      }
      if(!(compDecks(s1.deck[i], s2.deck[i], s1.deckCount[i]))) {
         res = 0;
      }
      if(i != currPlayer) {
         if((s1.handCount[i]) != (s2.handCount[i])) {
         res = 0;
         }
         if(!(compDecks(s1.hand[i], s2.hand[i], s1.handCount[i]))) {
            res = 0;
         }
         if((s1.discardCount[i]) != (s2.discardCount[i])) {
            res = 0;
         }
         if(!(compDecks(s1.discard[i], s2.discard[i], s1.discardCount[i]))) {
            res = 0;
         }
      }
   }
   return res;
}

// Checks the changes in gameState that should have been caused by the Mine 
// card. If the changes are valid, it returns 1. If not, it returns 0.
int checkMineChanges(struct gameState originalState,
                     struct gameState finalState, int currPlayer,
                     int trashedCard, int gainedCard)
{
   int i, j, n = 0, res = 1, numDiff = 0, currCard, chosenCard;
   int sOrgnl, sFnl, cOrgnl, cFnl;
   int mineCounter = 0, gainedCardCounter = 0, trashedCardCounter = 0;
   int orgnlHndCnt = originalState.handCount[currPlayer];
   int fnlHndCnt = finalState.handCount[currPlayer];
   int orgnlPlydCnt = originalState.playedCardCount;
   int fnlPlydCnt = finalState.playedCardCount;
   int orgnlTmpHnd[fnlHndCnt - 1], fnlTmpHnd[fnlHndCnt - 1];

   if(originalState.numActions != (finalState.numActions + 1)) {
      res = 0;
   }
   if((orgnlPlydCnt + 1) != fnlPlydCnt) {
      res = 0;
   }
   if(finalState.playedCards[orgnlPlydCnt] != mine) {
      res = 0;
   }
   // Determine which treasure card was chosen from the supply
   for(i = copper; i <= gold; i++) {
      sOrgnl = originalState.supplyCount[i];
      sFnl = finalState.supplyCount[i];
      if(sOrgnl != sFnl) {
         if(sOrgnl < sFnl) {
            res = 0;
            break; 
         }
         else {
            numDiff += sOrgnl - sFnl;
            chosenCard = i;
         }
      }
   }
   if(numDiff != 1) {
      res = 0;
   }
   if(chosenCard != gainedCard) {
      res = 0;
   }
   // Compare the original hand and the final hand
   // First take the Mine card and the trashed card out of the original hand
   j = 0;
   for(i = 0; i < orgnlHndCnt; i++) {
      currCard = originalState.hand[currPlayer][i];
      if((currCard == mine) && (mineCounter == 0)) {
         mineCounter++;
      }
      else if((currCard == trashedCard) && (trashedCardCounter == 0)) {
         trashedCardCounter++;
      }
      else {
         orgnlTmpHnd[j] = currCard;
         j++;
      }
   }
   // Next take the gained card out of the the final hand
   j = 0;
   for(i = 0; i < fnlHndCnt; i++) {
      currCard = finalState.hand[currPlayer][i];
      if((currCard == gainedCard) && (gainedCardCounter == 0)) {
         gainedCardCounter++;
      }
      else {
         fnlTmpHnd[j] = currCard;
         j++;
      }
   }
   // Now sort the temp hands to aid in comparison
   qsort((void *)orgnlTmpHnd, fnlHndCnt - 1, sizeof(int), compareMine);
   qsort((void *)fnlTmpHnd, fnlHndCnt - 1, sizeof(int), compareMine);
   if(!(compDecks(orgnlTmpHnd, fnlTmpHnd, fnlHndCnt - 1))) {
      res = 0;
   }
   // Make sure the chosen card only costs up to 3 more than the trashed card
   if(getCost(gainedCard) > (getCost(trashedCard) + 3)) {
      res = 0;
   }
   return res;
}

int randomTestGenerator(struct gameState *state1, struct gameState *state2,
                         int *handChoice, int *supplyChoice)
{
   int i, j, currPlayer, numPlayer, numHand, handPos, numDeck, numDiscard;
   int numPlayed, possChoices[6];

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
   // between -5 and 100 and simiarly for embargo tokens. For treasure cards,
   // choose a random number between 1 and 100
   for(i = 0; i <= treasure_map; i++) {
      state1->embargoTokens[i] = (rand() % 106) - 5;
      if((i == copper) || (i == silver) || (i == gold)) {
         state1->supplyCount[i] = (rand() % 100) + 1;
      }
      else {
         state1->supplyCount[i] = (rand() % 106) - 5;
      }
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

   // Choose a random number for number of buys between -1 and 5
   state1->numBuys = (rand() % 7) - 1;

   // Populate the possChoices[] array
   // Since the player should try to gain a treasure card using Mine, I
   // will make sure half the choices are treasure cards. The other choices
   // can be anything from 0 to treasure_map
   for(i = 0; i < 5; i++) {
      if(i < 3) {
         possChoices[i] = i + copper;
      }
      else {
         possChoices[i] = rand() % (treasure_map + 1);
      }
   }

   // For the last possible choice, choose an invalid card of -1 or 
   // treasure_map + 1
   if(rand() % 2) {
      possChoices[5] = -1;
   }
   else {
      possChoices[5] = treasure_map + 1;
   }

   // Randomly select the card that the player will attempt to gain with the
   // Mine card
   i = rand() % (sizeof(possChoices) / sizeof(int));
   *supplyChoice = possChoices[i];

   // Choose a random number of cards in the current player's hand between 2
   // and MAX_HAND_COUNT
   numHand = (rand() % MAX_HAND_COUNT) + 2;
   state1->handCount[currPlayer] = numHand;

   // Put random cards in the current player's hand. Since the player needs to 
   // give up a Treasure card to play Mine, I will preferentially choose 
   // Treasure cards for the hand, such that at least half of the cards in the 
   // hand are Treasure cards. One of the cards in the hand will be replaced
   // by the Mine card.
   for(i = 0; i < numHand; i++) {
      if(i % 2) {
         state1->hand[currPlayer][i] = (rand() % 3) + copper;
      }
      else {
         state1->hand[currPlayer][i] = rand() % (treasure_map + 1);
      }
   }

   // Choose a random hand position for the Mine card
   handPos = rand() % numHand;
   state1->hand[currPlayer][handPos] = mine;

   // Choose a random card from the hand to trash
   do {
      i = rand() % numHand;
   } while(i == handPos);
   *handChoice = i;

   // If numPlayer is greater than MAX_PLAYERS, then make it equal to 
   // MAX_PLAYERS so that the arrays don't go out of bounds for the following
   // tasks
   if(numPlayer > MAX_PLAYERS) {
      numPlayer = MAX_PLAYERS;
   }

   // For the other players, choose a random number of cards for their hands
   // between 0 and MAX_HAND_COUNT and populate the hands with random cards
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

void testMine(int seed) {
   int i, j, handPos, res, res1, res2, invalidChoice;
   int numPlayer, currPlayer, numHand, trashCardPos, trashedCard, gainedCard;
   struct gameState state, copyState;

   srand(seed);

   invalidChoice = 0;
   handPos = randomTestGenerator(&state, &copyState, &trashCardPos,
                                 &gainedCard);
   numPlayer = state.numPlayers;
   currPlayer = state.whoseTurn;
   numHand = state.handCount[currPlayer];
   trashedCard = state.hand[currPlayer][trashCardPos];

   // Determine if the card to be trashed or the card to be gained are invalid 
   // They are invalid if they are not Treasure cards or if the gained card 
   // costs more than 3 greater than the trashed card
   if((trashedCard != copper) && (trashedCard != silver) &&
      (trashedCard != gold))
   {
      invalidChoice = 1;
   }

   else if((gainedCard != copper) && (gainedCard != silver) &&
           (gainedCard != gold))
   {
      invalidChoice = 1;
   }

   else if((getCost(gainedCard)) > ((getCost(trashedCard)) + 3)) {
      invalidChoice = 2;
   }

   // Play the Mine card
   res = playCard(handPos, trashCardPos, gainedCard, 0, &state);

   if(invalidChoice > 0) {
      if(res != -1) {
         printf("Incorrect result\n");
      }
      else {
         printf("Correct result\n");
      }
   }
   // If the phase is incorrect, then check to see if -1 is returned
   else if(copyState.phase != 0) {
      if(res != -1) {
         printf("Incorrect result\n");
      }
      else {
         printf("Correct result\n");
      }
   }
   // If the number of actions is 0, then check to see if -1 is returned
   else if(copyState.numActions < 1) {
      if(res != -1) {
         printf("Incorrect result\n");
      }
      else {
         printf("Correct result\n");
      }
   }
   else {
      if(res == -1) {
         printf("Incorrect result\n");
      }
      else {
         // Check the resulting state for errors
         res1 = compStates(copyState, state, currPlayer);
         res2 = checkMineChanges(copyState, state, currPlayer, trashedCard,
                                 gainedCard);
         if((res1 == 0) || (res2 == 0)) {
            printf("Incorrect result\n");
         }
         else {
            printf("Correct result\n");
         }
      }
   }
}

int main(int argc, char *argv[]) {
   int seed;

   seed = atoi(argv[1]);
   testMine(seed);
   return 0;
}
