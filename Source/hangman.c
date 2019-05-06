/* La HangMan, by Joshua Gregory
See hangman.h for further comments and licence        
*/

#include "hangman.h"

//vs computer words.
//Feel free to edit these words, full capitals are advised but not required.
char const word_array[9][10] = {"LA_FORTUNA", "HANGMAN", "COMP_SCI", "KLAUS", "COMP_SYS", "GAMING", "MINECRAFT", "FORTNITE", "DAB"};

ISR(TIMER3_COMPA_vect) {
    scan_switches();
    scan_encoder();
}

//draw interrupt
ISR(INT6_vect) {
    switch(game_state) {
        case STATE_HOME:
            draw_home_screen();
            break;
        case STATE_PLAYING:
            draw_playing();
            break;
        case STATE_INPUT_WORD:
            draw_input_word();
            break;
        case STATE_GAME_END:
            draw_game_end();
            break;
    }
}

//movement interrupt
ISR(TIMER1_COMPA_vect) {
    switch(game_state) {
        case STATE_HOME:
            home_screen_movement();
            break;
        case STATE_PLAYING:
            playing_movement();
            break;
        case STATE_INPUT_WORD:
            input_word_movement();
            break;
        case STATE_GAME_END:
            game_end_movement();
            break;
    }
}

void draw_home_screen(void) {
    uint8_t midPos = 85;
    uint8_t triangle_y;
    if(last_selected_item == selected_item) return;
    clear_screen();
    display_string_xy("V3.0", 10, 10);
    display_string_xy("Welcome to La Hangman!", 85, 20);
    draw_hangman_full();
    display_string_xy("By Joshua Gregory", 85, 200);
    //display_uint16_xy(rand(), 85, 32); //displays the random number generated
    display_string_xy_col("VS Computer", midPos, 90, selected_item == 0 ? BLUE : WHITE);
    display_string_xy_col("VS Human", midPos, 115, selected_item == 1 ? BLUE : WHITE);
    
    switch(selected_item) {
        case 0: triangle_y = 90;
                break;
        case 1: triangle_y = 115;
                break;
        default: return;
    }
    fill_image_pgm(midPos - 3 * 2, triangle_y, 3, 3, triangle_sprite);
    last_selected_item = selected_item;
}

void home_screen_movement(void) {
    static uint8_t tick = 9;
    if(tick == 10) {
        int8_t rotary = os_enc_delta();
        if(rotary < 0 && selected_item > 0)
            selected_item--;
        else if(rotary > 0)
            selected_item = (selected_item + 1) % HOME_SCREEN_ITEMS;
        tick = 0;
    }
    tick++;
    
    if(get_switch_short(_BV(SWC))) {
        clear_switches();
        switch(selected_item) {
            case 0:
                clear_screen();
                is_drawn = FALSE;
                vs_comp = TRUE;
                game_state = STATE_PLAYING;
                break;
            case 1: 
                clear_screen();
                is_drawn = FALSE;
                game_state = STATE_INPUT_WORD;
                break;
        }
    }
}

void draw_playing(void) {
    draw_keyboard_no_word();
    drawn_char = FALSE;
    if(is_drawn) return;
    display_string_xy("Guess the word", 95, 20);
    display_string_xy("Incorrect letters:", 30, 105);
    is_drawn = TRUE;
}

void playing_movement(void) {
    uint8_t i = move_keyboard_char();
    //if character is selected
    if(i == 1) {
        //sets letter
        letter = k_str[0];
        reset_k_str();
    }
    //if back button pressed
    if(i == 2) {
        //quits to home screen
        clear_screen();
        game_state = STATE_HOME;
    }
}

void draw_input_word(void) {
    //if enter pressed
    if(move_keyboard()) {
        set_word(k_str);
        reset_k_str_full();
        clear_screen();
        is_drawn = FALSE;
        vs_comp = FALSE;
        game_state = STATE_PLAYING;
    }
    if(is_drawn) return;
    init_keyboard();
    is_drawn = FALSE;
}

void input_word_movement(void) {
    draw_keyboard();
    if(is_drawn) return;
    display_string_xy("Enter a word to guess!", 95, 20);
    display_string_xy("Word:", 30, 50);
    is_drawn = TRUE;
}

void draw_game_end(void) {
    if(is_drawn) return;
    clear_screen();
    if(word_guessed == TRUE) {
        display_string_xy("You win!", 30, 50);
        LED_ON;
    } else {
        display_string_xy("You lost!", 30, 50);
    }
    display_string_xy("The word was", 30, 59);
    display_string_xy(word_to_guess, 50, 80);
    display_string_xy("Press centre button to go back to menu", 30, 100);
    is_drawn = TRUE;
}

void game_end_movement(void) {
    if(get_switch_short(_BV(SWC))) { 
        clear_screen();
        last_selected_item = -1; // Force redraw of home screen
        selected_item = 0;
        clear_switches();
        game_state = STATE_HOME;
    }
}

//adapted method
uint16_t rand_init(void) {
    //ADC conversion from unused pins should give random results.
    //an amplification factor of 200x is used.
    ADMUX |= _BV(REFS0) | _BV(MUX3) | _BV(MUX1) | _BV(MUX0);
    //Prescaler
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1);
    //Enable and start
    ADCSRA |= _BV(ADEN) | _BV(ADSC);
    //Wait until complete
    while(! (ADCSRA & _BV(ADIF))) {
        _delay_ms(2);
    }
    //Read result
    uint16_t res = ADCL;
    res |= ADCH << 8;
    //Disable
    ADCSRA &= ~_BV(ADEN);
    
    //The XOR should increase the randomness?
    //since the converted number is only 10 bits
    return res ^ 0xACE1u;
}

//adapted method
//http://en.wikipedia.org/wiki/Linear_feedback_shift_register
uint16_t rand(void) {
    unsigned lsb = random_seed & 1;  /* Get lsb (i.e., the output bit). */
    random_seed >>= 1;               /* Shift register */
    if (lsb == 1)             /* Only apply toggle mask if output bit is 1. */
        random_seed ^= 0xB400u;        /* Apply toggle mask, value has 1 at bits corresponding
                             * to taps, 0 elsewhere. */
    return random_seed;
}

//adapted method
void os_init(void) {
	/* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;
	LED_INIT;
    init_lcd();
    init_encoder();
    init_keyboard();
    /* Frame rate */
	set_frame_rate_hz(31); /* > 60 Hz  (KPZ 30.01.2015) */
	/* Enable tearing interrupt to get flicker free display */
	EIMSK |= _BV(INT6);
	/* Enable game timer interrupt (Timer 1 CTC Mode 4) */
	TCCR1A = 0;
    //CTC: TOP=OCR1A; don't use TOV1 (it's set at MAX=0xffff)
	TCCR1B = _BV(WGM12);
	TCCR1B |= _BV(CS11); // clk/8
	TIMSK1 |= _BV(OCIE1A);
	/* Enable performance counter (Timer 3 CTC Mode 4) */
	TCCR3A = 0;
	TCCR3B = _BV(WGM32);
	TCCR3B |= _BV(CS31); // clk/8
	TIMSK3 |= _BV(OCIE3A);
	OCR3A = 2000;       // trigger interrupt every 2ms
    
    random_seed = rand_init();
}

//returns true (1) if val is in arrIn
uint8_t is_value_inarray(char val, char *arrIn) {
    char arr[27];
    strcpy(arr, arrIn);
    for (int i = 0; i < sizeof(arr)/sizeof(arr[0]); i++) {
        if (arr[i] == val) return TRUE;
    }
    return FALSE;
}

//sets all characters 'c' in guessing_word to '@'
//'@' counts as the current correctly guessed letters in the word
void update_guessing_word(char c) {
    for (int i = 0; i < sizeof(guessing_word)/sizeof(guessing_word[0]); i++) {
        if (guessing_word[i] == c) {
            guessing_word[i] = '@';
        }
    }
}

//sets all spaces ('_') in guessing_word to '@'
//spaces count as already correctly guessed characters
void update_spaces(void) {
    for (int i = 0; i < sizeof(guessing_word)/sizeof(guessing_word[0]); i++) {
        if (guessing_word[i] == '_') {
            guessing_word[i] = '@';
        }
    }
}

//checks if all the character in guessing_word are '@'
//if so, the word is fully guessed
uint8_t is_word_guessed(void) {
    for (int i = 0; i < sizeof(guessing_word)/sizeof(guessing_word[0]); i++) {
        if (guessing_word[i] != '@') {
            return FALSE;
        }
    }
    return TRUE; 
}

//method to "randomly" pick a word from the word_array
void pick_rand_word(void) {
    int8_t i = rand() % 100 % sizeof(word_array)/sizeof(word_array[0]);
    set_word(word_array[i]);
}

void set_word(char *word) {
    strncpy(word_to_guess, word, 10);
    strncpy(guessing_word, word, 10);
}

//method to draw the guessed character to the screen
//to the right of "Incorrect letters:" when playing
void draw_guessed_char(char c) {
    if(!is_value_inarray(c, guessed_chars)) { 
        if(drawn_char == FALSE) {
            display_string_xy(&c, guessed_line, 115);
            guessed_line += 10;
            append(guessed_chars, c);
            drawn_char = TRUE;
        }
    }
}

void append(char* s, char c) {
    int len = strlen(s);
    s[len] = c;
    s[len+1] = '\0';
}

//correctly displays the guessing lines of the word to guess
//shows spaces as well
void display_guessing_lines(void) {
    int x = 30;
    for (int i = 0; i < sizeof(word_to_guess)/sizeof(word_to_guess[0]); i++) {
        if(word_to_guess[i] != '\0') {
            if(word_to_guess[i] == '_') {
                display_string_xy(" ", x, 53);
                x += 10;
            } else {
                display_string_xy("_", x, 53);
                x += 10;
            }
        }
    }
}

//displays the character in the correct place above th guessing lines, if this character is guessed
void update_word_lines(char c) {
    for (int i = 0; i < sizeof(guessing_word)/sizeof(guessing_word[0]); i++) {
        int x = 30;
        if (guessing_word[i] == '@') {
            char p = word_to_guess[i];
            x = (x + (i * 10));
            if(p != '_') {
                display_string_xy(&p, x, 50);
            }
        }
    }
}

//array setup
void setup_guessed_chars(void) {
    for (int i = 0; i < sizeof(guessed_chars)/sizeof(guessed_chars[0]); i++) {
        guessed_chars[i] = '\0';
    }
}

//11 hangman pieces
void draw_hangman_piece(void) {
    switch(hang_counter) {
        case 1:
            display_string_xy("_____________", 200, 100);
            break;
        case 2:
            display_string_xy("|", 270, 23);
            display_string_xy("|", 270, 30);
            display_string_xy("|", 270, 37);
            display_string_xy("|", 270, 44);
            display_string_xy("|", 270, 51);
            display_string_xy("|", 270, 58);
            display_string_xy("|", 270, 65);
            display_string_xy("|", 270, 72);
            display_string_xy("|", 270, 79);
            display_string_xy("|", 270, 86);
            display_string_xy("|", 270, 93);
            display_string_xy("|", 270, 100);
            break;
        case 3:
            display_string_xy("__________", 215, 23);
            break;
        case 4:
            display_string_xy("\\", 266, 30);
            break;
        case 5:
            display_string_xy("|", 215, 30);
            break;
        case 6:
            display_string_xy("_", 214, 32);
            display_string_xy("()", 211, 39);
            //display_string_xy(")", 218, 39);
            display_string_xy("_", 214, 39);
            
            display_string_xy("|", 215, 30);
            break;
        case 7:
            display_string_xy("|", 215, 46);
            display_string_xy("|", 215, 53);
            display_string_xy("|", 215, 58);
            break;
        case 8:
            display_string_xy("/", 211, 51);
            break;
        case 9:
            display_string_xy("\\", 218, 51);
            break;
        case 10:
            display_string_xy("/", 212, 64);
            break;
        case 11:
            display_string_xy("\\", 217, 64);
            break;
    }
}

void draw_hangman_full(void) {
    display_string_xy("_____________", 200, 120);
    display_string_xy("|", 270, 43);
    display_string_xy("|", 270, 50);
    display_string_xy("|", 270, 57);
    display_string_xy("|", 270, 64);
    display_string_xy("|", 270, 71);
    display_string_xy("|", 270, 78);
    display_string_xy("|", 270, 85);
    display_string_xy("|", 270, 92);
    display_string_xy("|", 270, 99);
    display_string_xy("|", 270, 106);
    display_string_xy("|", 270, 113);
    display_string_xy("|", 270, 120);
    display_string_xy("__________", 215, 43);
    display_string_xy("\\", 266, 50);
    display_string_xy("|", 215, 50);
    display_string_xy("_", 214, 52);
    display_string_xy("()", 211, 59);
    display_string_xy("_", 214, 59);
    display_string_xy("|", 215, 50);
    display_string_xy("|", 215, 66);
    display_string_xy("|", 215, 73);
    display_string_xy("|", 215, 78);
    display_string_xy("/", 211, 71);
    display_string_xy("\\", 218, 71);
    display_string_xy("/", 212, 84);
    display_string_xy("\\", 217, 84);
}    
    
void main(void) {
    os_init();
    do {
        game_state = STATE_HOME;
        last_selected_item = -1;
        OCR1A = 6192;
        LED_OFF;
        sei();
        init_keyboard();
        //home screen loop
        while(game_state != STATE_PLAYING);
        //playing loop
        while(game_state == STATE_PLAYING){
            if(vs_comp == TRUE){
                pick_rand_word();
            }
            word_guessed = FALSE;
            game_end = FALSE;
            display_guessing_lines();
            update_spaces();
            letter = '\0';
            setup_guessed_chars();
            init_keyboard();
            drawn_char = FALSE;
            guessed_line = 30;
            hang_counter = 0;
            do {
                if(is_value_inarray(letter, guessing_word)){
                    //display_string_xy("Correct letter!  ", 30, 80);
                    update_guessing_word(letter);
                    update_word_lines(letter);
                    append(guessed_chars, letter);
                    //display_string_xy(word_to_guess, 230, 115);
                    //display_string_xy(guessing_word, 140, 115);
                    letter = '\0';
                    _delay_ms(400);
                } else {
                    if(!is_value_inarray(letter, guessed_chars)) {
                        //display_string_xy("Incorrect letter!", 30, 80);
                        draw_guessed_char(letter);
                        hang_counter++;
                        draw_hangman_piece();
                        letter = '\0';
                    }
                }
                if(is_word_guessed()){
                    word_guessed = TRUE;
                    display_string_xy("Word guessed!      ", 30, 80);
                    _delay_ms(1000);
                    game_end = TRUE;
                }                    
                if(hang_counter == 11){
                    display_string_xy("Hangman complete! ", 30, 80);
                    _delay_ms(1500);
                    is_drawn = FALSE;
                    game_end = TRUE;
                }
            } while(game_end == FALSE && game_state != STATE_HOME);
            is_drawn = FALSE;
            game_state = STATE_GAME_END;
            while(game_state != STATE_HOME);
        }
        cli();
    } while(1);
}