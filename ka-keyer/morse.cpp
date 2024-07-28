#include "morse.h"

// Recommendation ITU-R M.1677-1
// https://www.itu.int/dms_pubrec/itu-r/rec/m/R-REC-M.1677-1-200910-I!!PDF-E.pdf

const char morse_0[] PROGMEM = "-----";
const char morse_1[] PROGMEM = ".----";
const char morse_2[] PROGMEM = "..---";
const char morse_3[] PROGMEM = "...--";
const char morse_4[] PROGMEM = "....-";
const char morse_5[] PROGMEM = ".....";
const char morse_6[] PROGMEM = "-....";
const char morse_7[] PROGMEM = "--...";
const char morse_8[] PROGMEM = "---..";
const char morse_9[] PROGMEM = "----.";
const char morse_a[] PROGMEM = ".-";
const char morse_b[] PROGMEM = "-...";
const char morse_c[] PROGMEM = "-.-.";
const char morse_d[] PROGMEM = "-..";
const char morse_e[] PROGMEM = ".";
const char morse_f[] PROGMEM = "..-.";
const char morse_g[] PROGMEM = "--.";
const char morse_h[] PROGMEM = "....";
const char morse_i[] PROGMEM = "..";
const char morse_j[] PROGMEM = ".---";
const char morse_k[] PROGMEM = "-.-";
const char morse_l[] PROGMEM = ".-..";
const char morse_m[] PROGMEM = "--";
const char morse_n[] PROGMEM = "-.";
const char morse_o[] PROGMEM = "---";
const char morse_p[] PROGMEM = ".--.";
const char morse_q[] PROGMEM = "--.-";
const char morse_r[] PROGMEM = ".-.";
const char morse_s[] PROGMEM = "...";
const char morse_t[] PROGMEM = "-";
const char morse_u[] PROGMEM = "..-";
const char morse_v[] PROGMEM = "...-";
const char morse_w[] PROGMEM = ".--";
const char morse_x[] PROGMEM = "-..-";
const char morse_y[] PROGMEM = "-.--";
const char morse_z[] PROGMEM = "--..";
//                                             1234567
const char morse_period[]           PROGMEM = ".-.-.-"; // .
const char morse_comma[]            PROGMEM = "--..--"; // ,
const char morse_colon[]            PROGMEM = "---..."; // :
const char morse_questionmark[]     PROGMEM = "..--.."; // ?
const char morse_apostrophe[]       PROGMEM = ".----."; // '
const char morse_dash[]             PROGMEM = "-....-"; // -
const char morse_bar[]              PROGMEM = "-..-.";  // /
const char morse_leftparenthesis[]  PROGMEM = "-.--.";  // (
const char morse_rightparenthesis[] PROGMEM = "-.--.-"; // )
const char morse_quotationmark[]    PROGMEM = ".-..-."; // "
const char morse_doubledash[]       PROGMEM = "-...-";  // =
const char morse_cross[]            PROGMEM = ".-.-.";  // +
const char morse_at[]               PROGMEM = ".--.-."; // @

const char morse_space[]            PROGMEM = " ";      // Space
const char morse_null[]             PROGMEM = "";

const char *const table_ascii2morse[] PROGMEM = {
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 00 - 07
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 08 - 0F
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 10 - 17
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 18 - 1F
  morse_space,           morse_null,             morse_quotationmark, morse_null,      morse_null,       morse_null,       morse_null,      morse_apostrophe,   // 20 - 27
  morse_leftparenthesis, morse_rightparenthesis, morse_null,          morse_cross,     morse_comma,      morse_dash,       morse_period,    morse_bar,          // 28 - 2F
  morse_0,               morse_1,                morse_2,             morse_3,         morse_4,          morse_5,          morse_6,         morse_7,            // 30 - 37
  morse_8,               morse_9,                morse_colon,         morse_null,      morse_null,       morse_doubledash, morse_null,      morse_questionmark, // 38 - 3F
  morse_at,              morse_a,                morse_b,             morse_c,         morse_d,          morse_e,          morse_f,         morse_g,            // 40 - 47
  morse_h,               morse_i,                morse_j,             morse_k,         morse_l,          morse_m,          morse_n,         morse_o,            // 48 - 4F
  morse_p,               morse_q,                morse_r,             morse_s,         morse_t,          morse_u,          morse_v,         morse_w,            // 50 - 57
  morse_x,               morse_y,                morse_z,             morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 58 - 5F
  morse_null,            morse_a,                morse_b,             morse_c,         morse_d,          morse_e,          morse_f,         morse_g,            // 60 - 67
  morse_h,               morse_i,                morse_j,             morse_k,         morse_l,          morse_m,          morse_n,         morse_o,            // 68 - 6F
  morse_p,               morse_q,                morse_r,             morse_s,         morse_t,          morse_u,          morse_v,         morse_w,            // 70 - 77
  morse_x,               morse_y,                morse_z,             morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 78 - 7F
  morse_null,            morse_null,             morse_null,          morse_null,      morse_null,       morse_null,       morse_null,      morse_null,         // 80 - 87
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

// This table has been cretaed by morse::begin
const char table_morse2ascii[] PROGMEM = {
   0, 69, 84, 73, 65, 78, 77, 83, 85, 82, 87, 68, 75, 71, 79, 72, 
  86, 70,  0, 76,  0, 80, 74, 66, 88, 67, 89, 90, 81,  0,  0, 53, 
  52,  0, 51,  0,  0,  0, 50,  0,  0, 43,  0,  0,  0,  0, 49, 54, 
  61,  0,  0,  0,  0, 40,  0, 55,  0,  0,  0, 56,  0, 57, 48,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 63,  0,  0,  0,  0, 
   0, 34,  0,  0, 46,  0,  0,  0,  0, 64,  0,  0,  0, 39,  0,  0, 
  45,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 41,  0,  0,  0, 
   0,  0, 44,  0,  0,  0,  0, 58,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// void Morse::begin(){
//   uint8_t table[256];
//   char buffer[10];

//   // Make sure the table is zeroed
//   for (uint16_t n = 0; n < 256; n++) table[n] = 0;

//   for (uint8_t ascii = 0x7F; ascii > 0x1F; ascii--){
//     strcpy_P(buffer, encode(ascii));
//     uint8_t value = getIndex(buffer);
//     table[value] = ascii;
//   }

//   Serial.println("const char table_morse2ascii[] PROGMEM = {");
//   for (uint8_t h = 0; h < 16; h++){
//     for (uint8_t l = 0; l < 16; l++){
//       uint8_t i = (h << 4) | l;
//       Serial.print(table[i]);
//       Serial.print(", ");
//     }
//     Serial.println("");
//   }
//   Serial.println("};");
// }


// Computes unique number for a given Morse sequence
uint8_t Morse::getIndex(const char* sequence){
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

const char* Morse::encode(char c){
  return pgm_read_ptr(&table_ascii2morse[(uint8_t)c]);
}

char Morse::decode(char *buffer){
  return pgm_read_byte(&table_morse2ascii[getIndex(buffer)]);
}
