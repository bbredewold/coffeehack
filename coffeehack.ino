byte z0, z1, z2, z3;
byte x0, x1, x2, x3, x4;
byte d0, d1, d2, d3;
byte intra = 1, inter = 7;
String hexval;
int count;

const char *recipeNames[] = {
    "espresso",     "ristretto",    "coffee",       "cappuccino",       "latte_macchiato",      "macchiato",    "milkcoffee",       "frothed_milk",
    "espresso-x2",  "ristretto-x2", "coffee-x2",    "cappuccino-x2",    "latte_macchiato-x2",   "macchiato-x2", "milkcoffee-x2",    "frothed_milk-x2"
};

// Recipe Memory Address, Previous Count, New Count
int recipes[][3] = {
    {0x280, 0, 0},  {0x281, 0, 0},  {0x282, 0, 0},  {0x284, 0, 0},      {0x285, 0, 0},          {0x286, 0, 0},  {0x288, 0, 0},      {0x289, 0, 0},
    {0x291, 0, 0},  {0x292, 0, 0},  {0x293, 0, 0},  {0x295, 0, 0},      {0x296, 0, 0},          {0x297, 0, 0},  {0x299, 0, 0},      {0x29A, 0, 0}
};

void setup() {
  Serial1.begin(9600);
}

void loop() {
  
    for (int i=0; i < sizeof(recipes)/sizeof(recipes[0]); i++) {
        // Set Previous New Count to Previous Count
        recipes[i][1] = recipes[i][2];
        // Set New count;
        recipes[i][2] = getCounter(recipes[i][0]);
        // Check if recipe is greater then 0. Sometimes the 0 value will result in a -1.
        if (recipes[i][1] > 0) {
            // If New Count equal to Previous Count + 1, call Webhook
            if (recipes[i][2] == recipes[i][1] + 1) callWebhook(recipeNames[i], recipes[i][1], recipes[i][2]);
        }
    }

    delay(3 * 1000);
}

void callWebhook(String recipe, int countOld, int countNew)
{
  Particle.publish("coffeemaker/makecoffee", recipe, 60, PRIVATE);
}

int getCounter(int offset)
{
  if(offset > 0x800) return -1;

  hexval = String(offset, HEX);
  while(hexval.length() < 3) hexval = "0" + hexval;

  toCoffeemaker('R'); delay(inter);
  toCoffeemaker('E'); delay(inter);
  toCoffeemaker(':'); delay(inter);
  toCoffeemaker('0'); delay(inter);
  toCoffeemaker(hexval[0]); delay(inter);
  toCoffeemaker(hexval[1]); delay(inter);
  toCoffeemaker(hexval[2]); delay(inter);
  toCoffeemaker(0x0D); delay(inter);
  toCoffeemaker(0x0A); delay(100);

  String r = "";

  while(Serial1.available()) {
    delay (intra); d0 = Serial1.read();
    delay (intra); d1 = Serial1.read();
    delay (intra); d2 = Serial1.read();
    delay (intra); d3 = Serial1.read();
    delay (inter);
    r += char(fromCoffeemaker(d0,d1,d2,d3));
  }

  if (r.length() == 9) {
    String hex = r.substring(3,7);
    int number = (int)strtol(hex.c_str(), NULL, 16);
    return number;
  } else {
    return -1;
  }
}

byte fromCoffeemaker(byte x0, byte x1, byte x2, byte x3) {
  bitWrite(x4, 0, bitRead(x0,2));
  bitWrite(x4, 1, bitRead(x0,5));
  bitWrite(x4, 2, bitRead(x1,2));
  bitWrite(x4, 3, bitRead(x1,5));
  bitWrite(x4, 4, bitRead(x2,2));
  bitWrite(x4, 5, bitRead(x2,5));
  bitWrite(x4, 6, bitRead(x3,2));
  bitWrite(x4, 7, bitRead(x3,5));
  return x4;
}

byte toCoffeemaker(byte z) {
  z0 = 255;
  z1 = 255;
  z2 = 255;
  z3 = 255;

  bitWrite(z0, 2, bitRead(z,0));
  bitWrite(z0, 5, bitRead(z,1));
  bitWrite(z1, 2, bitRead(z,2));
  bitWrite(z1, 5, bitRead(z,3));
  bitWrite(z2, 2, bitRead(z,4));
  bitWrite(z2, 5, bitRead(z,5));
  bitWrite(z3, 2, bitRead(z,6));
  bitWrite(z3, 5, bitRead(z,7));

  delay(intra); Serial1.write(z0);
  delay(intra); Serial1.write(z1);
  delay(intra); Serial1.write(z2);
  delay(intra); Serial1.write(z3);
  delay(inter);
}
