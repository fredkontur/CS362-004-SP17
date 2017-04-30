/******************************************************************************
 * Filename:     testSuite.c
 * Author:       Fred Kontur
 * Date Written: April 26, 2016
 * Last Edited:  April 26, 2016
 * Description:  This file runs the tests from the unittest1.c through 
 *               unittest4.c and cardtest1.c through cardtest4.c files.
******************************************************************************/

void testShuffle();
void testInitializeGame();
void testEndTurn();
void testIsGameOver();
void testSmithy();
void testAdventurer();
void testFeast();
void testMine();

int main(int argc, char *argv[]) {
   testShuffle();
   testInitializeGame();
   testEndTurn();
   testIsGameOver();
   testSmithy();
   testAdventurer();
   testFeast();
   testMine();

   return 0;
}
