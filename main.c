#include "bitmaps.h"
#include "Buttons.h"

void Atualiza_Luta(int, int);
void Result_Screen(int);
uint8_t* HP_Bars(int);
uint8_t* Escolha(int, int);
uint8_t* Escolha_Aleatoria(time_t, struct tm*, int);
int Round(uint8_t*, uint8_t*);
void Instructions();
void Start_Fight();

int main(void) {
//------------Initial config------------
    Nokia5110_Init();
    Nokia5110_Clear();
    SysCtlClockSet(SYSCTL_SYSDIV_1|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    ConfigureButtons();
    Nokia5110_Clear();

    //start game loop
    while(1){
        //show title screen
        while( GetButton() == BUTTON_NOT_PRESSED ){
            Nokia5110_DrawFullImage(game_title);
            SysCtlDelay( (SysCtlClockGet())/15 );
        }

        SysCtlDelay( (SysCtlClockGet())/15 );
        while(1){
        //game start menu
            Nokia5110_Clear();
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP(5, 25, Cursor_Start_Menu, 0);
            Nokia5110_DisplayBuffer();

            Nokia5110_SetCursor(2,2);
            Nokia5110_OutString("Instrucoes");

            Nokia5110_SetCursor(2,3);
            Nokia5110_OutString("Novo Jogo");

            int state = 5;
            int loop = 1;

            SysCtlDelay( (SysCtlClockGet())/20 );
            while(GetButton() !=  13){ //confirm button SW3: first row, third column
                SysCtlDelay( (SysCtlClockGet())/20 );
                //wait for a selection in the game start menu

                if(GetButton() == 14){ //skill choice button
                    SysCtlDelay( (SysCtlClockGet())/20 );
                    if(loop == 2){ //trocar pra 3
                        state = 0; //trocar pra 5
                        loop = 0; //trocar pra 1
                    }

                    Nokia5110_Clear();
                    Nokia5110_ClearBuffer();

                    Nokia5110_PrintBMP(5, 25 + state, Cursor_Start_Menu, 0);
                    Nokia5110_DisplayBuffer();

                    Nokia5110_SetCursor(2,2);
                    Nokia5110_OutString("Instrucoes");

                    Nokia5110_SetCursor(2,3);
                    Nokia5110_OutString("Novo Jogo");

                    loop++;
                    state += 5;
                }
            }

            SysCtlDelay( (SysCtlClockGet())/20 );

            if(loop == 1){
                //Instructions screen
                Instructions();
                SysCtlDelay( (SysCtlClockGet())/20 );
            }

            else if(loop == 2){
                //Start Game Screen
                Start_Fight();
                SysCtlDelay( (SysCtlClockGet())/20 );
            }
        }
    }
}


// This function shows how to play the game
void Instructions(){
    Nokia5110_Clear();
    Nokia5110_ClearBuffer();
    Nokia5110_SetCursor(1,1);
    Nokia5110_OutString("Commands");

    Nokia5110_SetCursor(0,3);
    Nokia5110_OutString("SW3 -Confirm");

    Nokia5110_SetCursor(0,4);
    Nokia5110_OutString("SW4 -Cursor");

    SysCtlDelay( (SysCtlClockGet())/10 );
    while(GetButton() != 13){
    }

    Nokia5110_Clear();
    Nokia5110_ClearBuffer();
    SysCtlDelay( (SysCtlClockGet())/10 );
    Nokia5110_DrawFullImage(Instructions_screen);

    while(GetButton() != 13){
        SysCtlDelay( (SysCtlClockGet())/20 );
    }
}

void Start_Fight(){
    //-----------Time configuration --------
    // Used to get random moves for oponent
    time_t tempo = time(NULL);
    struct tm* timePtr = localtime(&tempo);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
    HibernateEnableExpClk( SysCtlClockGet() );
    HibernateRTCSet(tempo);
    HibernateRTCEnable();
    int segundos = timePtr->tm_sec;

    int HP_BAR1 = 0;
    int HP_BAR2 = 0;
    int HP_aux; //to help store data from whose player i need less the health points
    uint8_t* Player_choice = 0; //each player's turn choice
    uint8_t* Oponent_choice = 0; //each random turn choice

    //sets a new fight
    HP_BAR1 = 3;
    HP_BAR2 = 3;

    Nokia5110_Clear();
    Nokia5110_ClearBuffer();

    //starts fight
    while(HP_BAR1!=0 && HP_BAR2!=0){

        //call choices
        Player_choice = Escolha(HP_BAR1, HP_BAR2);
        Oponent_choice = Escolha_Aleatoria(tempo, timePtr, segundos);

        //show skills choices, and get some delay to help user view
        Nokia5110_DisplayBuffer();

        //show skills transition
        Nokia5110_PrintBMP(32, 30, VERSUS, 0);
        Nokia5110_DisplayBuffer();

        SysCtlDelay( (SysCtlClockGet())/2 );

        //decides round winner, and less round loser health points
        HP_aux = Round(Player_choice, Oponent_choice);
        if(HP_aux == 1)
            HP_BAR2--;
        else if(HP_aux == 0)
            HP_BAR1--;


        if(HP_BAR1 == 0)
            Result_Screen(0);

        else if(HP_BAR2 == 0)
            Result_Screen(1);
    }
}

//This function decides who won the round
//0 for lose, 1 for win, 2 for draw
int Round(uint8_t* player, uint8_t* oponent){

    //player chooses first skill
    if(player==SKILL_1 && oponent==SKILL_1){
        return 2;
    }
    else if(player==SKILL_1 && oponent==SKILL_2){
        return 0;
    }
    else if(player==SKILL_1 && oponent==SKILL_3){
        return 1;
    }

    //player chooses second skill
    if(player==SKILL_2 && oponent==SKILL_1){
        return 1;
    }
    else if(player==SKILL_2 && oponent==SKILL_2){
        return 2;
    }
    else if(player==SKILL_2 && oponent==SKILL_3){
        return 0;
    }

    //player chooses third skill
    if(player==SKILL_3 && oponent==SKILL_1){
        return 0;
    }
    else if(player==SKILL_3 && oponent==SKILL_2){
        return 1;
    }
    else if(player==SKILL_3 && oponent==SKILL_3){
        return 2;
    }
}

uint8_t* Escolha(int hp1, int hp2){
    int state = 17;
    int loop = 1;

    SysCtlDelay( (SysCtlClockGet())/15 );
    Nokia5110_Clear();
    Nokia5110_ClearBuffer();

    Nokia5110_PrintBMP(15, 47, Skill_Cursor, 0);
    Nokia5110_DisplayBuffer();
    Atualiza_Luta(hp1, hp2);

    while(GetButton() !=  13){ //confirm button SW3: first row, third column

        if(GetButton() == 14){ //skill choice button
            if(loop == 3){
                state = 0;
                loop = 0;
            }
            Nokia5110_Clear();
            Nokia5110_ClearBuffer();

            Nokia5110_PrintBMP(15 + state, 47, Skill_Cursor, 0);
            Nokia5110_DisplayBuffer();
            Atualiza_Luta(hp1, hp2);

            loop++;
            state+=17;
        }
    }

    //clear screen and return player choice
    Nokia5110_Clear();
    Nokia5110_ClearBuffer();
    if(loop == 1){
        Nokia5110_PrintBMP(10, 30, SKILL_1_TEMP, 0);
        return SKILL_1;
    }
    else if(loop == 2){
        Nokia5110_PrintBMP(10, 30, SKILL_2_TEMP, 0);
        return SKILL_2;
    }
    else{
        Nokia5110_PrintBMP(10, 30, SKILL_3_TEMP, 0); //10,30
        return SKILL_3;
    }

}

uint8_t* Escolha_Aleatoria(time_t tempo, struct tm* timePtr, int segundos){
    int choice = 0;
    //insert random generator
    tempo = HibernateRTCGet();
    timePtr = localtime( &tempo );

    segundos = timePtr->tm_sec;
    if(segundos%5 == 0)
        choice = 1;
    else if(segundos%2 == 0)
        choice = 2;
    else
        choice = 3;

    //returns random choice for oponent
    if(choice==1){
        Nokia5110_PrintBMP(55, 30, SKILL_1_TEMP, 0);
        return SKILL_1;
    }
    else if (choice==2){
        Nokia5110_PrintBMP(55, 30, SKILL_2_TEMP, 0);
        return SKILL_2;
    }
    else{
        Nokia5110_PrintBMP(55, 30, SKILL_3_TEMP, 0);
        return SKILL_3;
    }
}

void Atualiza_Luta(int hp1, int hp2){

    //player fighter and his health points
    Nokia5110_PrintBMP(0, 7, HP_Bars(hp1) , 0);
    Nokia5110_PrintBMP(7, 32, Ryu, 0);


    //opponent fighter and his health points
    Nokia5110_PrintBMP(52, 7, HP_Bars(hp2) , 0);
    Nokia5110_PrintBMP(63, 32, Zangief, 0);

    //skills bar
    Nokia5110_PrintBMP(17, 45, SKILL_1, 0);
    Nokia5110_PrintBMP(34, 45, SKILL_2, 0);
    Nokia5110_PrintBMP(34+17, 45, SKILL_3, 0);

    //show bitmaps on screen
    Nokia5110_DisplayBuffer();

    SysCtlDelay( (SysCtlClockGet())/15 );
}

void Result_Screen(int final){
    Nokia5110_Clear();
    if(final==0)
        Nokia5110_DrawFullImage(FINAL_DERROTA);
    else if(final==1)
        Nokia5110_DrawFullImage(FINAL_VITORIA);
    SysCtlDelay( (SysCtlClockGet())/3 );
}

uint8_t* HP_Bars(int hp){
    switch(hp){
    case 0:
        return ZeroHP_Bar;
    case 1:
        return LowHP_Bar;
    case 2:
        return MedHP_Bar;
    case 3:
        return FullHP_Bar;
    }
}
