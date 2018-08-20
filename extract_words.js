const fs = require('fs');

const arrs = {
  0: [],
  1: [],
  2: [],
  3: [],
  4: [],
  5: [],
  6: [],
  7: [],
  8: []
};

var words = fs.readFileSync('./words.txt').toString().split('\n');
for (const word of words) {
  if (word.length >=2 && word.length <=8 && arrs[word.length].indexOf(word.toUpperCase()) < 0) {
	  arrs[word.length].push(word.toUpperCase());
  }
}

const lengths = [];
let total = 0;
for (let i=0; i<=8; i++) {
	lengths[i] = arrs[i].length;
	total += lengths[i];
}

// OUTPUT
console.log(`#ifndef WORDS_H
#define WORDS_H

// Source: https://github.com/Xethron/Hangman/blob/master/words.txt

const uint16_t WORD_COUNT = ${total};

const uint8_t WORD_COUNTS[] = {${lengths.join(',')}};

const char WORDS[] PROGMEM = {`);

for (let i=2; i<=8; i++) {
  for (const word of arrs[i]) {
    console.log(`  '${word.split('').join("','")}',`);
  }
}

console.log(`};

#endif`);
