#include "morse.hpp"

// Recommendation ITU-R M.1677-1
// https://www.itu.int/dms_pubrec/itu-r/rec/m/R-REC-M.1677-1-200910-I!!PDF-E.pdf

static const char morse_0[] PROGMEM = "-----";
static const char morse_1[] PROGMEM = ".----";
static const char morse_2[] PROGMEM = "..---";
static const char morse_3[] PROGMEM = "...--";
static const char morse_4[] PROGMEM = "....-";
static const char morse_5[] PROGMEM = ".....";
static const char morse_6[] PROGMEM = "-....";
static const char morse_7[] PROGMEM = "--...";
static const char morse_8[] PROGMEM = "---..";
static const char morse_9[] PROGMEM = "----.";
static const char morse_a[] PROGMEM = ".-";
static const char morse_b[] PROGMEM = "-...";
static const char morse_c[] PROGMEM = "-.-.";
static const char morse_d[] PROGMEM = "-..";
static const char morse_e[] PROGMEM = ".";
static const char morse_f[] PROGMEM = "..-.";
static const char morse_g[] PROGMEM = "--.";
static const char morse_h[] PROGMEM = "....";
static const char morse_i[] PROGMEM = "..";
static const char morse_j[] PROGMEM = ".---";
static const char morse_k[] PROGMEM = "-.-";
static const char morse_l[] PROGMEM = ".-..";
static const char morse_m[] PROGMEM = "--";
static const char morse_n[] PROGMEM = "-.";
static const char morse_o[] PROGMEM = "---";
static const char morse_p[] PROGMEM = ".--.";
static const char morse_q[] PROGMEM = "--.-";
static const char morse_r[] PROGMEM = ".-.";
static const char morse_s[] PROGMEM = "...";
static const char morse_t[] PROGMEM = "-";
static const char morse_u[] PROGMEM = "..-";
static const char morse_v[] PROGMEM = "...-";
static const char morse_w[] PROGMEM = ".--";
static const char morse_x[] PROGMEM = "-..-";
static const char morse_y[] PROGMEM = "-.--";
static const char morse_z[] PROGMEM = "--..";
//                                                    1234567
static const char morse_period[]           PROGMEM = ".-.-.-"; // .
static const char morse_comma[]            PROGMEM = "--..--"; // ,
static const char morse_colon[]            PROGMEM = "---..."; // :
static const char morse_semicolon[]        PROGMEM = "-.-.-."; // ;
static const char morse_exclamationmark[]  PROGMEM = "-.-.--"; // !
static const char morse_ampersand[]        PROGMEM = ".-..." ; // &
static const char morse_questionmark[]     PROGMEM = "..--.."; // ?
static const char morse_apostrophe[]       PROGMEM = ".----."; // '
static const char morse_dash[]             PROGMEM = "-....-"; // -
static const char morse_bar[]              PROGMEM = "-..-.";  // /
static const char morse_leftparenthesis[]  PROGMEM = "-.--.";  // (
static const char morse_rightparenthesis[] PROGMEM = "-.--.-"; // )
static const char morse_quotationmark[]    PROGMEM = ".-..-."; // "
static const char morse_doubledash[]       PROGMEM = "-...-";  // =
static const char morse_cross[]            PROGMEM = ".-.-.";  // +
static const char morse_at[]               PROGMEM = ".--.-."; // @
static const char morse_enye[]             PROGMEM = "--.--";  // Ñ
static const char morse_cedilla[]          PROGMEM = "-.-..";  // Ç

static const char morse_space[]            PROGMEM = " ";      // Space
static const char morse_null[]             PROGMEM = "";

static const char *const table_ascii2morse[] PROGMEM = {
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 00 - 07
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 08 - 0F
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 10 - 17
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 18 - 1F
  morse_space,           morse_exclamationmark,  morse_quotationmark, morse_null,      morse_null,       morse_null,       morse_ampersand, morse_apostrophe,   // 20 - 27
  morse_leftparenthesis, morse_rightparenthesis, morse_null,          morse_cross,     morse_comma,      morse_dash,       morse_period,    morse_bar,          // 28 - 2F
  morse_0,               morse_1,                morse_2,             morse_3,         morse_4,          morse_5,          morse_6,         morse_7,            // 30 - 37
  morse_8,               morse_9,                morse_colon,         morse_semicolon, morse_null,       morse_doubledash, morse_null,      morse_questionmark, // 38 - 3F
  morse_at,              morse_a,                morse_b,             morse_c,         morse_d,          morse_e,          morse_f,         morse_g,            // 40 - 47
  morse_h,               morse_i,                morse_j,             morse_k,         morse_l,          morse_m,          morse_n,         morse_o,            // 48 - 4F
  morse_p,               morse_q,                morse_r,             morse_s,         morse_t,          morse_u,          morse_v,         morse_w,            // 50 - 57
  morse_x,               morse_y,                morse_z,             morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 58 - 5F
  morse_null,            morse_a,                morse_b,             morse_c,         morse_d,          morse_e,          morse_f,         morse_g,            // 60 - 67
  morse_h,               morse_i,                morse_j,             morse_k,         morse_l,          morse_m,          morse_n,         morse_o,            // 68 - 6F
  morse_p,               morse_q,                morse_r,             morse_s,         morse_t,          morse_u,          morse_v,         morse_w,            // 70 - 77
  morse_x,               morse_y,                morse_z,             morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 78 - 7F
  morse_enye,            morse_enye,             morse_cedilla,       morse_cedilla,   morse_null,       morse_null,       morse_null,      morse_null,         // 80 - 87
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 88 - 8F
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 90 - 97
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 98 - 9F
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // A0 - A7
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // A8 - AF
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // B0 - B7
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // B8 - BF
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // C0 - C7
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // C8 - CF
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // D0 - D7
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // D8 - DF
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // E0 - E7
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // E8 - EF
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // F0 - F7
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null          // F8 - FF
};

static uint8_t table_morse2ascii[256];

// Computes unique number for a given Morse sequence
static uint8_t getIndex(const char* sequence) {
  uint8_t index = 0;
  while (*sequence){
    switch (*sequence){
      case '.':
        index = (index << 1) + 1;
      break;
      case '-':
        index = (index << 1) + 2;
      break;
    }
    sequence++;
  }
  return index;
}

const char* morseEncode(char c) {
  return (const char*)pgm_read_ptr(&table_ascii2morse[(uint8_t)c]);
}

char morseDecode(char *buffer) {
  return table_morse2ascii[getIndex(buffer)];
}

void morseBegin(){
  uint8_t index;
  uint8_t ascii;
  char buffer[10];
  
  // Compute the decoding table in
  // reverse order (important!)
  for (int n = 255; n >= 0; n--){
    ascii = (uint8_t)n;
    strcpy_P(buffer, morseEncode(ascii));
    index = getIndex(buffer);
    table_morse2ascii[index] = ascii;
  }
}

