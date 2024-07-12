bool capslock = false; // todo: query for capslock state
bool shift = false;
const uint16_t data_port = 0x60;
const uint16_t status_port = 0x64;

/* Credits to https://github.com/BurntRanch/badOS */
char kbd [128] = {
  0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', -1,   
  '\t', /* <-- Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
  0, /* <-- control key */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  -4, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', -4,
  '*',
  0,  /* Alt */
  ' ',  /* Space bar */
  -3,  /* Caps lock */
  0,  /* 59 - F1 key ... > */
  0,   0,   0,   0,   0,   0,   0,   0,
  0,  /* < ... F10 */
  0,  /* 69 - Num lock*/
  0,  /* Scroll Lock */
  0,  /* Home key */
  0,  /* Up Arrow */
  0,  /* Page Up */
  '-',
  0,  /* Left Arrow */
  0,
  0,  /* Right Arrow */
  '+',
  -2,  /* 79 - End key*/
  0,  /* Down Arrow */
  0,  /* Page Down */
  0,  /* Insert Key */
  0,  /* Delete Key */
  0,   0,   0,
  0,  /* F11 Key */
  0,  /* F12 Key */
  0,  /* All other keys are undefined */	};

char caps_kbd [128] = {
	0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', -5,   
  '\t', /* <-- Tab */
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
  0, /* <-- control key */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',  0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', -8,
  '*',
  0,  /* Alt */
  ' ',  /* Space bar */
  -7,  /* Caps lock */
  0,  /* 59 - F1 key ... > */
  0,   0,   0,   0,   0,   0,   0,   0,
  0,  /* < ... F10 */
  0,  /* 69 - Num lock*/
  0,  /* Scroll Lock */
  0,  /* Home key */
  0,  /* Up Arrow */
  0,  /* Page Up */
  '-',
  0,  /* Left Arrow */
  0,
  0,  /* Right Arrow */
  '+',
  -6,  /* 79 - End key*/
  0,  /* Down Arrow */
  0,  /* Page Down */
  0,  /* Insert Key */
  0,  /* Delete Key */
  0,   0,   0,
  0,  /* F11 Key */
  0,  /* F12 Key */
  0,  /* All other keys are undefined */	};
