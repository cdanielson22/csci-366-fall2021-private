//
// Created by carson on 5/20/20.
//

#include <stdlib.h>
#include <stdio.h>
#include "game.h"

// STEP 9 - Synchronization: the GAME structure will be accessed by both players interacting
// asynchronously with the server.  Therefore the data must be protected to avoid race conditions.
// Add the appropriate synchronization needed to ensure a clean battle.

static game * GAME = NULL;

void game_init() {
    if (GAME) {
        free(GAME);
    }
    GAME = malloc(sizeof(game));
    GAME->status = CREATED;
    game_init_player_info(&GAME->players[0]);
    game_init_player_info(&GAME->players[1]);
}

void game_init_player_info(player_info *player_info) {
    player_info->ships = 0;
    player_info->hits = 0;
    player_info->shots = 0;
}

int game_fire(game *game, int player, int x, int y) {
    // Step 5 - This is the crux of the game.  You are going to take a shot from the given player and
    // update all the bit values that store our game state.
    //
    //  - You will need up update the players 'shots' value
    //  - you You will need to see if the shot hits a ship in the opponents ships value.  If so, record a hit in the
    //    current players hits field
    //  - If the shot was a hit, you need to flip the ships value to 0 at that position for the opponents ships field
    //
    //  If the opponents ships value is 0, they have no remaining ships, and you should set the game state to
    //  PLAYER_1_WINS or PLAYER_2_WINS depending on who won.
    int firedAt;
    if (player == 1){
        firedAt = 0;
    } else {
        firedAt = 1;
    }
    player_info *playerFiring = &game->players[player];

    player_info *playerFiringAt = &game->players[firedAt];

    if (playerFiringAt->ships == 0){ // make sure that there are ships loaded to fire at
        return 0;
    }

    if (x < 0 || x > 7){ // make sure that the x and y are valid
        return 0;
    }
    if (y < 0 || y > 7){
        return 0;
    }

    unsigned long long int mask = xy_to_bitval(x, y);

    playerFiring->shots |= mask;
    if (player == 0){
        GAME->status = PLAYER_1_TURN;
    } else {
        GAME->status = PLAYER_0_TURN;
    }

    if (mask & playerFiringAt->ships){
        playerFiring->hits |= mask;
        if (player == 0){
            if (playerFiringAt->ships == playerFiring->hits){
                GAME->status = PLAYER_0_WINS;
            }
        }
        if (playerFiringAt->ships == playerFiring->hits){
            GAME->status = PLAYER_1_WINS;
        }
        return 1;
    } else {
        return 0;
    }

    /*
    player_info *playerFiring = &game->players[player];
    unsigned long long int mask = xy_to_bitval(x, y);
    playerFiring->shots;

    player_info  *playerBeingFiringAt = &game->players[?];
    playerBeingFiringAt->ships;
    playerFiring->hits;

    playerBeingFiringAt->ships;
    */

}

unsigned long long int xy_to_bitval(int x, int y) {
    // Step 1 - implement this function.  We are taking an x, y position
    // and using bitwise operators, converting that to an unsigned long long
    // with a 1 in the position corresponding to that x, y
    //
    // x:0, y:0 == 0b00000...0001 (the one is in the first position)
    // x:1, y: 0 == 0b00000...10 (the one is in the second position)
    // ....
    // x:0, y: 1 == 0b100000000 (the one is in the eighth position)
    //
    // you will need to use bitwise operators and some math to produce the right
    // value.

    // if statments to make sure that the x and y values are valid
    if (x < 0 || x > 7) {
        return 0;
    }
    if (y < 0 || y > 7){
        return 0;
    }
    // making a 64 bit integer and doing two bit shifts for the x and the y values
    unsigned long long int mask = 1ull;
    mask <<= x;
    mask <<= y*8;
    return mask; // i then return the integer that is a mask of teh x and y values


}

struct game * game_get_current() {
    return GAME;
}

int game_load_board(struct game *game, int player, char * spec) {
    // Step 2 - implement this function.  Here you are taking a C
    // string that  represents a layout of ships, then testing
    // to see if it is a valid layout (no off-the-board positions
    // and no overlapping ships)
    //

    // if it is valid, you should write the corresponding unsigned
    // long long value into the Game->players[player].ships data
    // slot and return 1
    //
    // if it is invalid, you should return -1
    // check to see if the spec is null and if it is return -1
    if (spec == NULL){
        return -1;
    }



    // make some varaibles to keep track of what is added i will change them to one when ships are added
    int carr = 0;
    int battle = 0;
    int dest = 0;
    int sub = 0;
    int pat = 0;
    //variable to track the amount of ships added
    int amount = 0;


    // getting the player info for the player that is passed in
    player_info *playerInfo = &game->players[player];


    // for loop that goes through all the ships that are given by spec
    for (int i = 0; i < 15; i+=3){
        // parts of the spec are number that represent ascii characters.
        // the first on is a letter that represents the type of ship that it is and if its horizoltal or if its vertical.
        // the next two postions are the row and then the column that the ships is located at
        char ship = spec[i];
        char row = spec[i+1];
        char col = spec[i+2];

        // i do some math to get the row and column to the right size. i minus 48 because thats were 0 is in the ascii table
        col -= 48;
        row -= 48;

        // once i have hte values to proper size in convert them to ints so they can be passed to the add ship functions later
        int colInt = (int)(col);
        int rowInt = (int)(row);

        // all these if statements are basically the same is just change vales
        // teh first if is to determine what ship is being added and what is orintation is
        if (ship == 99) { // carrier
            // this first if checks to see if the ship is addable and the ship should be added when i call the function for the if statement
            if (add_ship_vertical(playerInfo, rowInt, colInt, 5) == -1){ return -1; }
            // this if to so check if the ship type has already been added
            if (carr == 1) { return -1; }
            // at the end if the checks are passed then i could the ship type up and the total amount up once
            carr = 1;
            amount++;
        }

        if (ship == 67) { // carrier
            if (add_ship_horizontal(playerInfo, rowInt, colInt, 5) == -1){ return -1; }
            if (carr == 1){ return -1; }
            carr = 1;
            amount++;
        }

        if (ship == 98) { // battleship
            if (add_ship_vertical(playerInfo, rowInt, colInt, 4) == -1){ return -1; }
            if (battle == 1) { return -1; }
            battle = 1;
            amount++;
        }

        if (ship == 66) { // battleship
            if (add_ship_horizontal(playerInfo, rowInt, colInt, 4) == -1){ return -1; }
            if (battle == 1) { return -1; }
            battle = 1;
            amount++;
        }

        if (ship == 100) { // destoryer
            if(add_ship_vertical(playerInfo, rowInt, colInt, 3) == -1) { return -1; }
            if (dest == 1) { return -1; }
            dest = 1;
            amount++;
        }

        if (ship == 68) { // destoryer
            if(add_ship_horizontal(playerInfo, rowInt, colInt, 3) == -1){ return -1; }
            if (dest == 1){ return -1; }
            dest = 1;
            amount++;
        }

        if(ship == 115) { // sub
            if(add_ship_vertical(playerInfo, rowInt, colInt, 3) == -1){ return -1; }
            if(sub == 1){ return -1; }
            sub = 1;
            amount++;
        }

        if (ship == 83) { // sub
            if(add_ship_horizontal(playerInfo, rowInt, colInt, 3) == -1){ return -1; }
            if(sub == 1){ return -1; }
            sub = 1;
            amount++;
        }

        if(ship == 112){ // patorl
            if(add_ship_vertical(playerInfo, rowInt, colInt, 2) == -1) { return -1; }
            if(pat == 1) { return -1; }
            pat = 1;
            amount++;
        }

        if(ship == 80) { // patrol
            if(add_ship_horizontal(playerInfo, rowInt, colInt, 2) == -1){ return -1; }
            if(pat == 1){ return -1; }
            pat = 1;
            amount++;
        }
    }
    // at the end of those checks i check to make sure that there are 5 ships total
    if (amount != 5) {
        return -1;
    }
    // if all the checks are passed then I have a valid board and i return 1
    GAME->status = CREATED;
    if (player == 1){
        GAME->status = PLAYER_0_TURN;
    }
    return 1;

    /*
    char * current = spec[0];
    printf("%d", current);
    //char ship = *current;
    //char col = *(current +1);
    //char row = *(current + 2);

    int colInt = 0;
    int rowInt = 0;
    int length = 0;

    player_info *playerInfo = &game->players[player];

    //if(add_ship_horizontal(playerInfo, colInt, rowInt, length) == -1){
     //   return -1;
    //}
    return 0;*/
}

int add_ship_horizontal(player_info *player, int x, int y, int length) {
    // implement this as part of Step 2
    // returns 1 if the ship can be added, -1 if not
    // hint: this can be defined recursively

    // i first get the mask from the xy to bitval function
    unsigned long long int mask = xy_to_bitval(x, y);
    // i then check to make sure that i have been given valid numbers that arent off the board
    if (x < 0 || x > 8) {
        return -1;
    }
    if (y < 0 || y > 8){
        return -1;
    }
    // here is my recuresive function call
    if (length == 0) { // if the length is 0 that means the ships was added and the runction returns 1 for success
        return 1;
    }else if ((player->ships & mask)){
        // I check to make sure that there isnt an overlap with the position being added and the previous ships
        // if there is an overlap then i return -1 for failure
        return -1;
    } else {
        // if the check is passed then i added the xy postion into the ships and call the function again with the parameters incremented
        player->ships = player->ships | mask;
        return add_ship_horizontal(player, ++x, y, --length);
    }

    // need to do some recursion and the base case will be when length is 0
}

int add_ship_vertical(player_info *player, int x, int y, int length) {
    // implement this as part of Step 2
    // returns 1 if the ship can be added, -1 if not
    // hint: this can be defined recursively

    // the vertical is the same and horizontal but i increment on the y axis instead of the x axis
    unsigned long long int mask = xy_to_bitval(x, y);

    if (x < 0 || x > 8) {
        return -1;
    }
    if (y < 0 || y > 8){
        return -1;
    }

    if (length == 0){
        return  1;
    } else if ((player->ships & mask)){
        return -1;
    }else {
        player->ships = player->ships | mask;
        return add_ship_vertical(player, x, y + 1, length - 1);
    }

}